#include "nnue.h"

#include "position.h"
#include "simd.h"

struct Network net;

void NNUE_update(struct Accumulator* acc, struct Position* pos) {
    for (int pov = WHITE; pov <= BLACK; pov++) {
        struct Pov_Accumulator* povAccumulator = &(acc)->perspective[pov];
        struct Pov_Accumulator* previousPovAccumulator = &(acc - 1)->perspective[pov];

        // return early if we already updated this accumulator (aka it's "clean")
        if (Pov_Accumulator_isClean(povAccumulator))
            continue;
        // if the top accumulator needs a refresh, ignore all the other dirty accumulators and just refresh it
        else if (povAccumulator->needsRefresh) {
            Pov_Accumulator_refresh(povAccumulator, pos);
            continue;
        }
        // if the previous accumulator is clean, just UE on top of it and avoid the need to scan backwards
        else if (Pov_Accumulator_isClean(previousPovAccumulator)) {
            Pov_Accumulator_applyUpdate(povAccumulator, previousPovAccumulator);
            continue;
        }

        // if we can't update we need to start scanning backwards
        // if in our scan we'll find an accumulator that needs a refresh we'll just refresh the top one, otherwise we'll start an UE chain
        for (int UEableAccs = 1; UEableAccs < MAXPLY; UEableAccs++) {
            struct Pov_Accumulator* currentAcc = &(acc - UEableAccs)->perspective[pov];
            // check if the current acc should be refreshed
            if (currentAcc->needsRefresh) {
                Pov_Accumulator_refresh(povAccumulator, pos);
                break;
            }
            // If not check if it's a valid starting point for an UE chain
            else if (Pov_Accumulator_isClean(&(acc - UEableAccs - 1)->perspective[pov])) {
                for (int j = (pos->accumStackHead - 1 - UEableAccs); j <= pos->accumStackHead - 1; j++) {
                    Pov_Accumulator_applyUpdate(&pos->accumStack[j].perspective[pov], &pos->accumStack[j - 1].perspective[pov]);
                }
                break;
            }
        }
    }
}

void Pov_Accumulator_refresh(struct Pov_Accumulator* accumulator, struct Position* pos) {
    Pov_Accumulator_accumulate(accumulator, pos);
    // Reset the add and sub vectors for this accumulator, this will make it "clean" for future updates
    accumulator->NNUEAdd_size = 0;
    accumulator->NNUESub_size = 0;
    // mark any accumulator as refreshed
    accumulator->needsRefresh = false;
}

void Pov_Accumulator_addSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub) {
    const int16_t* const Add = &net.FTWeights[add * L1_SIZE];
    const int16_t* const Sub = &net.FTWeights[sub * L1_SIZE];
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = prev_acc->values[i] - Sub[i] + Add[i];
    }
}

void Pov_Accumulator_addSubSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub1, size_t sub2) {
    const int16_t* const Add = &net.FTWeights[add * L1_SIZE];
    const int16_t* const Sub1 = &net.FTWeights[sub1 * L1_SIZE];
    const int16_t* const Sub2 = &net.FTWeights[sub2 * L1_SIZE];
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = prev_acc->values[i] - Sub1[i] - Sub2[i] + Add[i];
    }
}

int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm) {
    // this function takes the net output for the current accumulators and returns the eval of the position
    // according to the net

    const int16_t* us = board_accumulator->perspective[stm].values;
    const int16_t* them = board_accumulator->perspective[stm ^ 1].values;
    return NNUE_ActivateFTAndAffineL1(us, them, &net.L1Weights[0], net.L1Biases[0]);
}

void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos) {
    for (int i = 0; i < 2; i++) {
        Pov_Accumulator_accumulate(&board_accumulator->perspective[i], pos);
    }
}

void Pov_Accumulator_applyUpdate(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* previousPovAccumulator) {

    assert(Pov_Accumulator_isClean(previousPovAccumulator));

    // return early if we already updated this accumulator (aka it's "clean")
    if (Pov_Accumulator_isClean(accumulator))
        return;

    // figure out what update we need to apply and do that
    const int adds = accumulator->NNUEAdd_size;
    const int subs = accumulator->NNUESub_size;

    // Quiets
    if (adds == 1 && subs == 1) {
        Pov_Accumulator_addSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0]);
    }
    // Captures
    else if (adds == 1 && subs == 2) {
        Pov_Accumulator_addSubSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0], accumulator->NNUESub[1]);
    }
    // Castling
    else {
        Pov_Accumulator_addSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0]);
        Pov_Accumulator_addSub(accumulator, accumulator, accumulator->NNUEAdd[1], accumulator->NNUESub[1]);
        // Note that for second addSub, we put acc instead of acc - 1 because we are updating on top of
        // the half-updated accumulator
    }

    // Reset the add and sub vectors for this accumulator, this will make it "clean" for future updates
    accumulator->NNUEAdd_size = 0;
    accumulator->NNUESub_size = 0;
}

void Pov_Accumulator_accumulate(struct Pov_Accumulator* accumulator, struct Position* pos) {
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = net.FTBiases[i];
    }

    const int kingSq = KingSQ(pos, accumulator->pov);
    const bool flip = get_file(KingSQ(pos, accumulator->pov)) > 3;

    for (int square = 0; square < 64; square++) {
        const bool input = pos->pieces[square] != EMPTY;
        if (!input) continue;
        const int Idx = Pov_Accumulator_GetIndex(accumulator, Position_PieceOn(pos, square), square, kingSq, flip);
        const int16_t* const Add = &net.FTWeights[Idx * L1_SIZE];
        for (int j = 0; j < L1_SIZE; j++) {
            accumulator->values[j] += Add[j];
        }
    }
}

int Pov_Accumulator_GetIndex(const struct Pov_Accumulator* accumulator, const int piece, const int square, const int kingSq, bool flip) {
    const size_t COLOR_STRIDE = 64 * 6;
    const size_t PIECE_STRIDE = 64;
    const int piecetype = GetPieceType(piece);
    const int pieceColor = Color[piece];
    int pieceColorPov = accumulator->pov == WHITE ? pieceColor : (pieceColor ^ 1);
    // Get the final indexes of the updates, accounting for hm
    int squarePov = accumulator->pov == WHITE ? (square ^ 0b111000) : square;
    if (flip) squarePov ^= 0b000111;
    size_t Idx = pieceColorPov * COLOR_STRIDE + piecetype * PIECE_STRIDE + squarePov;
    return Idx;
}

int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias) {
#if defined(USE_SIMD) && !NOSTDLIB
    vepi32 sum = vec_zero_epi32();
    const vepi16 Zero = vec_zero_epi16();
    const vepi16 One = vec_set1_epi16(FT_QUANT);
    int weightOffset = 0;
    const int16_t* both[] = { us, them };
    for (int both_index = 0; both_index < 2; both_index++) {
        for (int i = 0; i < L1_SIZE; i += CHUNK_SIZE) {
            const vepi16 input = vec_loadu_epi((const vepi16*)&both[both_index][i]);
            const vepi16 weight = vec_loadu_epi((const vepi16*)&weights[i + weightOffset]);
            const vepi16 clipped = vec_min_epi16(vec_max_epi16(input, Zero), One);

            // In squared clipped relu, we want to do (clipped * clipped) * weight.
            // However, as clipped * clipped does not fit in an int16 while clipped * weight does,
            // we instead do mullo(clipped, weight) and then madd by clipped.
            const vepi32 product = vec_madd_epi16(vec_mullo_epi16(clipped, weight), clipped);
            sum = vec_add_epi32(sum, product);
        }
        weightOffset += L1_SIZE;
    }

    return (vec_reduce_add_epi32(sum) / FT_QUANT + bias) * NET_SCALE / (FT_QUANT * L1_QUANT);

#else
    int sum = 0;
    int weightOffset = 0;

    const int16_t* both[] = { us, them };

    for (int both_index = 0; both_index < 2; both_index++) {
        for (int i = 0; i < L1_SIZE; ++i) {
            const int16_t input = both[both_index][i];
            const int16_t weight = weights[i + weightOffset];
            const int16_t clipped = input < 0 ? 0 : input > FT_QUANT ? FT_QUANT : input; // clamp(input, int16_t(0), int16_t(FT_QUANT));
            sum += (int16_t)(clipped * weight) * clipped;
        }

        weightOffset += L1_SIZE;
    }

    return (sum / FT_QUANT + bias) * NET_SCALE / (FT_QUANT * L1_QUANT);
#endif
}


SMALL void NNUE_init() {
    // open the nn file

#if KAGGLE
    const char* nnpath = "//kaggle_simulations//agent//nn.net";
#else
    const char* nnpath = "nn.net";
#endif
    int nn = open(nnpath, 0, 0644); // Default permissions: -rw-r--r--
    if (nn < 0)
    {
        puts("Unable to open NN file");
        exit(1);
    }

    const size_t len = NUM_INPUTS * L1_SIZE + L1_SIZE + L1_SIZE * 2 * OUTPUT_BUCKETS + OUTPUT_BUCKETS;

    int8_t *ptr = mmap(NULL, len * sizeof(int16_t), 1, 2, nn, 0);

    const int ft_size = NUM_INPUTS * L1_SIZE;
    const int blockSize = L1_SIZE * 4;
    const int blockCount = ft_size / blockSize;

    int currentIndex = 0;
    for(int blockIndex = 0; blockIndex < blockCount; blockIndex++) {
        int8_t divisor = *ptr;
        ptr++;

        if(divisor == 0) {
            for (int i = 0; i < blockSize; i++) {
                net.FTWeights[currentIndex++] = 0;
            }
        }
        else {
            for (int i = 0; i < blockSize; i++) {
                net.FTWeights[currentIndex++] = (int16_t)((int16_t)(*ptr) * divisor);
                ptr++;
            }
        }
    }

    int16_t* ptr2 = (int16_t*)ptr;
    for (int i = 0; i < L1_SIZE; i++) {
        net.FTBiases[i] = *ptr2;
        ptr2++;
    }
    for (int i = 0; i < L1_SIZE * 2 * OUTPUT_BUCKETS; i++) {
        net.L1Weights[i] = *ptr2;
        ptr2++;
    }
    net.L1Biases[0] = *ptr2; // Just assume 1 output bucket for now
}
