#pragma once

#if !NOSTDLIB
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>

uint64_t GetTimeMs();
#else

typedef long bool;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int64_t ssize_t;
typedef uint64_t size_t;

enum {
    stdin = 0,
    stdout = 1,
    stderr = 2
};

#define NULL ((void *)0)
#define true 1
#define false 0

#if !NDEBUG
#define assert(condition)                                                      \
  if (!(condition)) {                                                          \
    printf("Assert failed on line %i: ", __LINE__);                            \
    puts(#condition "\n");                                                     \
    _sys(60, 1, 0, 0);                                                         \
  }
#else
#define assert(condition)
#endif

#define printf(format, ...) _printf(format, (size_t[]){__VA_ARGS__})

ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3);
void exit(const int returnCode);
int strlen(const char* const restrict string);
void puts(const char* const restrict string);
void puts_nonewline(const char* const restrict string);
void fflush(int fd);
bool strcmp(const char* restrict lhs, const char* restrict rhs);
int atoi(const char* restrict string);
void _printf(const char* format, const size_t* args);
size_t GetTimeMs();
void* mmap(void* addr, size_t len, size_t prot, size_t flags, size_t fd, size_t offset);
void* malloc(size_t len);
ssize_t open(const char* const restrict pathname, const int flags, const int mode);
ssize_t fopen(const char* const restrict pathname, const char* const restrict mode);
char* strstr(const char* haystack, const char* needle);
void* memset(void* ptr, int value, size_t n);
double log(double x);
int read(int fd, void* data, int count);
char* fgets(char* string0, int count, int file);

#endif

#define NAME "Alexandria-7.1.1"

static const char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

typedef uint64_t Bitboard;
typedef uint16_t TTKey;
typedef uint64_t ZobristKey;
typedef uint32_t Move;
typedef uint16_t PackedMove;

static const Move NOMOVE = 0;
#define MAXPLY 128
static const int MAXDEPTH = MAXPLY;
static const int MATE_SCORE = 32000;
static const int MATE_FOUND = MATE_SCORE - MAXPLY;
static const int SCORE_NONE = 32001;
static const int MAXSCORE = 32670;
static const Bitboard fullCheckmask = 0xFFFFFFFFFFFFFFFF;

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// Lookup to get the file of a square
static uint8_t get_file(const int square);

#define get_antidiagonal(sq) (get_rank[sq] + get_file(sq))

// encode pieces
enum {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    EMPTY = 14
};

// sides to move (colors)
enum {
    WHITE,
    BLACK,
    BOTH
};

enum {
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

enum {
    HFNONE,
    HFUPPER,
    HFLOWER,
    HFEXACT
};

// piece types
enum {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

// generate pawn attacks
static Bitboard MaskPawnAttacks(int side, int square);

// generate knight attacks
static Bitboard MaskKnightAttacks(int square);

// generate king attacks
static Bitboard MaskKingAttacks(int square);

static Bitboard pieceAttacks(int piecetype, int pieceSquare, Bitboard occ);

// set occupancies
static Bitboard SetOccupancy(int index, int bits_in_mask, Bitboard attack_mask);

// starts a bench for alexandria, searching a set of positions up to a set depth
static void StartBench(int depth);

// set/get/pop bit macros
static void set_bit(Bitboard* bitboard, const int square);
static int get_bit(const Bitboard bitboard, const int square);
static void pop_bit(Bitboard* bitboard, const int square);
static int GetLsbIndex(Bitboard bitboard);
static int popLsb(Bitboard* bitboard);
static int CountBits(Bitboard bitboard);

struct Position;

struct ScoredMove {
    Move move;
    int score;
};

// move list structure
struct MoveList {
    struct ScoredMove moves[256];
    int count;
};

enum Movetype {
    Quiet, doublePush, KSCastle,
    QSCastle, Capture, enPassant,
    knightPromo = 8, bishopPromo, rookPromo, queenPromo,
    knightCapturePromo, bishopCapturePromo, rookCapturePromo, queenCapturePromo
};

static Move encode_move(const int source, const int target, const int piece, const int movetype);

static int From(const Move move);
static int To(const Move move);
static int FromTo(const Move move);
static int Piece(const Move move);
static int PieceTo(const Move move);
static int PieceTypeTo(const Move move);
static int GetMovetype(const Move move);
static int getPromotedPiecetype(const Move move);
static bool isEnpassant(const Move move);
static bool isDP(const Move move);
static bool isCastle(const Move move);
static bool isCapture(const Move move);
static bool isQuiet(const Move move);
static bool isPromo(const Move move);
// Shorthand for captures + any promotion no matter if quiet or not 
static bool isTactical(const Move move);

struct Position;
struct SearchInfo;

// Parse a move from algebraic notation to the engine's internal encoding
static Move ParseMove(const char* move_string, struct Position* pos);
// parse UCI "position" command
static void ParsePosition(const char* command, struct Position* pos);

// parse UCI "go" command
static bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos);

// main UCI loop
static void UciLoop();

// Net arch: (768xINPUT_BUCKETS -> L1_SIZE)x2 -> 1xOUTPUT_BUCKETS
#define NUM_INPUTS 768
#define INPUT_BUCKETS 1
#define L1_SIZE 48
#define OUTPUT_BUCKETS 1

#define FT_QUANT 255
#define L1_QUANT 64
#define NET_SCALE 400

#if defined(USE_SIMD)
#define CHUNK_SIZE (sizeof(vepi16) / sizeof(int16_t))
#else
#define CHUNK_SIZE  1
#endif

typedef size_t NNUEIndices[2];
//using NNUEIndices = std::array<std::size_t, 2>;

struct Network {
    //int16_t FTWeights[INPUT_BUCKETS * NUM_INPUTS * L1_SIZE];
    //int16_t FTBiases[L1_SIZE];
    //int16_t L1Weights[L1_SIZE * 2 * OUTPUT_BUCKETS];
    //int16_t L1Biases[OUTPUT_BUCKETS];

    int16_t* FTWeights;
    int16_t* FTBiases;
    int16_t* L1Weights;
    int16_t* L1Biases;
};

#if !NOSTDLIB

#if defined(USE_SIMD)
#include <immintrin.h>
#endif

#if defined(USE_AVX512)
typedef __m512i vepi16;
typedef __m512i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);

#elif defined(USE_AVX2)
typedef __m256i vepi16;
typedef __m256i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);
#endif
#endif

struct Position;

// per pov accumulator
struct Pov_Accumulator {
    //std::array<int16_t, L1_SIZE> values;
    int16_t values[L1_SIZE];
    int pov;
    size_t NNUEAdd[2];
    int NNUEAdd_size;
    size_t NNUESub[2];
    int NNUESub_size;
    bool needsRefresh;
};

static void Pov_Accumulator_accumulate(struct Pov_Accumulator* accumulator, struct Position* pos);
static int Pov_Accumulator_GetIndex(const struct Pov_Accumulator* accumulator, const int piece, const int square, const int kingSq, bool flip);
static void Pov_Accumulator_addSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub);
static void Pov_Accumulator_addSubSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub1, size_t sub2);
static void Pov_Accumulator_applyUpdate(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* previousPovAccumulator);
static void Pov_Accumulator_refresh(struct Pov_Accumulator* accumulator, struct Position* pos);
static bool Pov_Accumulator_isClean(const struct Pov_Accumulator* accumulator);

// final total accumulator that holds the 2 povs
struct Accumulator {
    struct Pov_Accumulator perspective[2];
};

static void Accumulator_AppendAddIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
static void Accumulator_AppendSubIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
static void Accumulator_ClearAddIndex(struct Accumulator* accumulator);
static void Accumulator_ClearSubIndex(struct Accumulator* accumulator);

static void NNUE_init();
static void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos);
static void NNUE_update(struct Accumulator* acc, struct Position* pos);
static int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias);
static int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm);

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) \
    __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

struct BoardState {
    int castlePerm;
    int capture;
    int enPas;
    int fiftyMove;
    int plyFromNull;
    Bitboard checkers;
    Bitboard checkMask;
    Bitboard pinned;
}; // stores a move and the state of the game before that move is made
// for rollback purposes

struct Position {
    int16_t pieces[64]; // array that stores for every square of the board what piece is on it

    int side; // what side has to move

    struct BoardState state;
    int hisPly; // total number of halfmoves
    // unique  hashkey  that encodes a board position
    ZobristKey posKey;
    ZobristKey pawnKey;
    ZobristKey whiteNonPawnKey;
    ZobristKey blackNonPawnKey;
    // stores the state of the board  rollback purposes
    int historyStackHead;
    struct BoardState history[MAXPLY];

    // Stores the zobrist keys of all the positions played in the game + the current search instance, used for 3-fold
    ZobristKey played_positions[2048];
    int played_positions_size;
    // std::vector<ZobristKey> played_positions = {}; // TODO: Consider restoring vector-like functionality

    // Occupancies bitboards based on piece and side
    Bitboard bitboards[12];
    Bitboard occupancies[2];

    struct Accumulator accumStack[MAXPLY];
    int accumStackHead;
};

// Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
static Bitboard Position_GetPieceColorBB(struct Position const* const position, const int piecetype, const int color);
static struct Accumulator* Position_AccumulatorTop(struct Position* position);
static Bitboard Position_Occupancy(const struct Position* const pos, const int occupancySide);
static int Position_PieceCount(const struct Position* const pos);
static int Position_PieceOn(const struct Position* const pos, const int square);
static ZobristKey Position_getPoskey(const struct Position* const pos);
static int Position_get50MrCounter(const struct Position* const pos);
static int Position_getCastlingPerm(const struct Position* const pos);
static int Position_getEpSquare(const struct Position* const pos);
static int Position_getPlyFromNull(const struct Position* const pos);
static Bitboard Position_getCheckers(const struct Position* const pos);
static Bitboard Position_getCheckmask(const struct Position* const pos);
static Bitboard Position_getPinnedMask(const struct Position* const pos);
static int Position_getCapturedPiece(const struct Position* const pos);
static void Position_ChangeSide(struct Position* const pos);

// Hold the data from the uci input to set search parameters and some search data to populate the uci output
struct SearchInfo {
    // search start time
    uint64_t starttime;
    // search time initial lower bound if present
    uint64_t stoptimeBaseOpt;
    // search time scaled lower bound if present
    uint64_t stoptimeOpt;
    // search time upper bound if present
    uint64_t stoptimeMax;
    // max depth to reach for depth limited searches
    int depth;
    int seldepth;
    // types of search limits
    bool timeset;

    uint64_t nodes;

    bool stopped;
};

static void ResetBoard(struct Position* pos);
static void ResetInfo(struct SearchInfo* info);
static ZobristKey GeneratePosKey(const struct Position* const pos);
static ZobristKey GeneratePawnKey(const struct Position* pos);
static ZobristKey GenerateNonPawnKey(const struct Position* pos, int side);

static Bitboard RayBetween(int square1, int square2);
static void UpdatePinsAndCheckers(struct Position* pos, const int side);

// Get on what square of the board the king of color c resides
static int KingSQ(const struct Position* pos, const int c);

// Parse a string of moves in coordinate format and plays them
static void parse_moves(const char* moves, struct Position* pos);

// Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
static Bitboard GetPieceBB(const struct Position* pos, const int piecetype);

// parse FEN string
static void ParseFen(const char* command, struct Position* pos);

// Return a piece based on the type and the color
static int GetPiece(const int piecetype, const int color);

// Returns the piece_type of a piece
static int GetPieceType(const int piece);

// Returns true if side has at least one piece on the board that isn't a pawn, false otherwise
static bool BoardHasNonPawns(const struct Position* pos, const int side);

static ZobristKey keyAfter(const struct Position* pos, const Move move);

static void saveBoardState(struct Position* pos);

static void restorePreviousBoardState(struct Position* pos);

// if we don't have enough material to mate consider the position a draw
static bool MaterialDraw(const struct Position* pos);

static int ScaleMaterial(const struct Position* pos, int eval);
static int EvalPositionRaw(struct Position* pos);

// position evaluation
static int EvalPosition(struct Position* pos);

struct Position;
struct SearchData;
struct SearchStack;
struct MoveList;

const static int HH_MAX = 8192;
const static int CAPTHIST_MAX = 16384;
const static int CORRHIST_WEIGHT_SCALE = 1024;
const static int CORRHIST_GRAIN = 256;
#define CORRHIST_SIZE 16384
const static int CORRHIST_MAX = 16384;

// Functions used to update the history heuristics
static void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode);

// Fuction that returns the history bonus
static int history_bonus(const int depth);

// Getters for the history heuristics
static int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move);
static int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move);
static int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move);
// Clean all the history tables
static void CleanHistories(struct SearchData* sd);
// Updates history heuristics for a single move
static void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);
static void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);

// Corrhist stuff
static void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff);
static int adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const int rawEval);

static Bitboard ReverseBits(Bitboard bitboard);
static Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask);
// get bishop attacks
static Bitboard GetBishopAttacks(int square, Bitboard occupancy);
// get rook attacks
static Bitboard GetRookAttacks(int square, Bitboard occupancy);
// get queen attacks
static Bitboard GetQueenAttacks(const int square, Bitboard occupancy);

struct ThreadData;

static void InitNewGame(struct ThreadData* td);

// init slider piece's attack tables
static void InitAttackTables();

static void InitAll();

// print move (for UCI purposes)
static void PrintMove(const Move move);

static void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos);

static void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos);

static void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square);

static void MakeMove(const bool UPDATE, const Move move, struct Position* pos);
// Reverts the previously played move
static void UnmakeMove(const Move move, struct Position* pos);
// makes a null move (a move that doesn't move any piece)
static void MakeNullMove(struct Position* pos);
// Reverts the previously played null move
static void TakeNullMove(struct Position* pos);

static bool next_token(const char* str, int* index, char* token);

enum MovegenType {
    MOVEGEN_NOISY = 0b01,
    MOVEGEN_QUIET = 0b10,
    MOVEGEN_ALL = 0b11
};

// is the square given in input attacked by the current given side
static bool IsSquareAttacked(const struct Position* pos, const int square, const int side);

// function that adds a (not yet scored) move to a move list
static void AddMoveNonScored(const Move move, struct MoveList* list);

// function that adds an (already-scored) move to a move list
static void AddMoveScored(const Move move, const int score, struct MoveList* list);

// Check for move pseudo-legality
static bool IsPseudoLegal(const struct Position* pos, const Move move);

// Check for move legality
static bool IsLegal(struct Position* pos, const Move move);

// generate moves
static void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type);

enum {
    PICK_TT,
    GEN_NOISY,
    PICK_GOOD_NOISY,
    PICK_KILLER,
    GEN_QUIETS,
    PICK_QUIETS,
    GEN_BAD_NOISY,
    PICK_BAD_NOISY
};

enum MovepickerType {
    SEARCH,
    QSEARCH
};

struct Movepicker {
    enum MovepickerType movepickerType;
    struct Position* pos;
    struct SearchData* sd;
    struct MoveList moveList;
    struct MoveList badCaptureList;
    Move ttMove;
    Move killer;
    int idx;
    int stage;
};

static void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode);
static Move NextMove(struct Movepicker* mp, const bool skip);

// convert ASCII character pieces to encoded constants
static int char_pieces(const char ch);

// Map promoted piece to the corresponding ASCII character
static char promoted_pieces(const int piece);

struct SearchStack {
    // don't init. search will init before entering the negamax method
    int16_t staticEval;
    Move excludedMove;
    Move move;
    int ply;
    Move searchKiller;
    int doubleExtensions;
};

struct SearchData {
    int searchHistory[2][64 * 64];
    int captHist[12 * 64][6];
    int pawnCorrHist[2][CORRHIST_SIZE];
    int whiteNonPawnCorrHist[2][CORRHIST_SIZE];
    int blackNonPawnCorrHist[2][CORRHIST_SIZE];
};

// a collection of all the data a thread needs to conduct a search
struct ThreadData {
    struct Position pos;
    struct SearchData sd;
    struct SearchInfo info;
    int RootDepth;
    int nmpPlies;
};

static void init_thread_data(struct ThreadData* td);

// ClearForSearch handles the cleaning of the thread data from a clean state
static void ClearForSearch(struct ThreadData* td);

// Starts the search process, this is ideally the point where you can start a multithreaded search
static void RootSearch(int depth, struct ThreadData* td);

// SearchPosition is the actual function that handles the search, it sets up the variables needed for the search , calls the Negamax function and handles the console output
static void SearchPosition(int start_depth, int final_depth, struct ThreadData* td);

// Sets up aspiration windows and starts a Negamax search
static int AspirationWindowSearch(int prev_eval, int depth, struct ThreadData* td);

static int Negamax(int alpha, int beta, int depth, const bool cutNode, struct ThreadData* td, struct SearchStack* ss);

static int Quiescence(int alpha, int beta, struct ThreadData* td, struct SearchStack* ss);

// inspired by the Weiss engine
static bool SEE(const struct Position* pos, const int move, const int threshold);

// Checks if the current position is a draw
static bool IsDraw(struct Position* pos);

static bool IsRepetition(const struct Position* pos); // remove this later, make static
static bool Is50MrDraw(struct Position* pos); // remove this later, make static

static void Optimum(struct SearchInfo* info, int time, int inc);
static bool StopEarly(const struct SearchInfo* info);
static bool TimeOver(const struct SearchInfo* info);

#define ENTRIES_PER_BUCKET 3

// 10 bytes:
// 2 for move
// 2 for score
// 2 for eval
// 2 for key
// 1 for depth
// 1 for age + bound + PV
PACK(struct TTEntry {
    PackedMove move;
    int16_t score;
    int16_t eval;
    TTKey ttKey;
    uint8_t depth;
    uint8_t ageBoundPV; // lower 2 bits is bound, 3rd bit is PV, next 5 is age
});

// Packs the 10-byte entries into 32-byte buckets
// 3 entries per bucket with 2 bytes of padding
struct TTBucket {
    struct TTEntry entries[ENTRIES_PER_BUCKET];
    uint16_t padding;
} __attribute__((aligned(32)));

#if !NOSTDLIB
static_assert(sizeof(struct TTEntry) == 10);
static_assert(sizeof(struct TTBucket) == 32);
#endif

struct TTable {
    struct TTBucket* pTable;
    uint64_t numBuckets;
    size_t paddedSize;
    uint8_t age;
};

static const uint8_t MAX_AGE = 1 << 5; // must be power of 2
static const uint8_t AGE_MASK = MAX_AGE - 1;

static void* AlignedMalloc(size_t size, size_t alignment);

static void AlignedFree(void* src);

static void TTEntry_init(struct TTEntry* const tte);
static void TTBucket_init(struct TTBucket* const ttb);

static void ClearTT();
// Initialize an TT of size MB
static void InitTT(uint64_t MB);

static bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte);

static void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, int eval, const int bound, const int depth, const bool pv, const bool wasPV);

static uint64_t Index(const ZobristKey posKey);

static void TTPrefetch(const ZobristKey posKey);

static int ScoreToTT(int score, int ply);

static int ScoreFromTT(int score, int ply);

static PackedMove MoveToTT(Move move);

static Move MoveFromTT(struct Position* pos, PackedMove packed_move);

static uint8_t BoundFromTT(uint8_t ageBoundPV);

static bool FormerPV(uint8_t ageBoundPV);

static uint8_t AgeFromTT(uint8_t ageBoundPV);

static uint8_t PackToTT(uint8_t bound, bool wasPV, uint8_t age);

static void UpdateTableAge();
