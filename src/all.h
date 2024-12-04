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

extern int see_margin[64][2];

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

// Lookup to get the rank of a square
extern const uint8_t get_rank[64];

// Lookup to get the file of a square
uint8_t get_file(const int square);

// Lookup to get the diagonal of a square
extern const uint8_t get_diagonal[64];

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

// Lookup to get the color from a piece
extern const int Color[12];
extern const int PieceType[12];
// Contains the material Values of the pieces
extern const int SEEValue[15];

// generate pawn attacks
Bitboard MaskPawnAttacks(int side, int square);

// generate knight attacks
Bitboard MaskKnightAttacks(int square);

// generate king attacks
Bitboard MaskKingAttacks(int square);

// pawn attacks table [side][square]
extern Bitboard pawn_attacks[2][64];

// knight attacks table [square]
extern Bitboard knight_attacks[64];

// king attacks table [square]
extern Bitboard king_attacks[64];

Bitboard pieceAttacks(int piecetype, int pieceSquare, Bitboard occ);

// set occupancies
Bitboard SetOccupancy(int index, int bits_in_mask, Bitboard attack_mask);

// starts a bench for alexandria, searching a set of positions up to a set depth
void StartBench(int depth);

// set/get/pop bit macros
void set_bit(Bitboard* bitboard, const int square);
int get_bit(const Bitboard bitboard, const int square);
void pop_bit(Bitboard* bitboard, const int square);
int GetLsbIndex(Bitboard bitboard);
int popLsb(Bitboard* bitboard);
int CountBits(Bitboard bitboard);

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

Move encode_move(const int source, const int target, const int piece, const int movetype);

int From(const Move move);
int To(const Move move);
int FromTo(const Move move);
int Piece(const Move move);
int PieceTo(const Move move);
int PieceTypeTo(const Move move);
int GetMovetype(const Move move);
int getPromotedPiecetype(const Move move);
bool isEnpassant(const Move move);
bool isDP(const Move move);
bool isCastle(const Move move);
bool isCapture(const Move move);
bool isQuiet(const Move move);
bool isPromo(const Move move);
// Shorthand for captures + any promotion no matter if quiet or not 
bool isTactical(const Move move);

struct Position;
struct SearchInfo;

// Parse a move from algebraic notation to the engine's internal encoding
Move ParseMove(const char* move_string, struct Position* pos);
// parse UCI "position" command
void ParsePosition(const char* command, struct Position* pos);

// parse UCI "go" command
bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos);

// main UCI loop
void UciLoop();

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

extern struct Network net;

#if !NOSTDLIB

#include "all.h"

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

void Pov_Accumulator_accumulate(struct Pov_Accumulator* accumulator, struct Position* pos);
int Pov_Accumulator_GetIndex(const struct Pov_Accumulator* accumulator, const int piece, const int square, const int kingSq, bool flip);
void Pov_Accumulator_addSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub);
void Pov_Accumulator_addSubSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub1, size_t sub2);
void Pov_Accumulator_applyUpdate(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* previousPovAccumulator);
void Pov_Accumulator_refresh(struct Pov_Accumulator* accumulator, struct Position* pos);
bool Pov_Accumulator_isClean(const struct Pov_Accumulator* accumulator);

// final total accumulator that holds the 2 povs
struct Accumulator {
    struct Pov_Accumulator perspective[2];
};

void Accumulator_AppendAddIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
void Accumulator_AppendSubIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
void Accumulator_ClearAddIndex(struct Accumulator* accumulator);
void Accumulator_ClearSubIndex(struct Accumulator* accumulator);

void NNUE_init();
void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos);
void NNUE_update(struct Accumulator* acc, struct Position* pos);
int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias);
int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm);

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
Bitboard Position_GetPieceColorBB(struct Position const* const position, const int piecetype, const int color);
struct Accumulator* Position_AccumulatorTop(struct Position* position);
Bitboard Position_Occupancy(const struct Position* const pos, const int occupancySide);
int Position_PieceCount(const struct Position* const pos);
int Position_PieceOn(const struct Position* const pos, const int square);
ZobristKey Position_getPoskey(const struct Position* const pos);
int Position_get50MrCounter(const struct Position* const pos);
int Position_getCastlingPerm(const struct Position* const pos);
int Position_getEpSquare(const struct Position* const pos);
int Position_getPlyFromNull(const struct Position* const pos);
Bitboard Position_getCheckers(const struct Position* const pos);
Bitboard Position_getCheckmask(const struct Position* const pos);
Bitboard Position_getPinnedMask(const struct Position* const pos);
int Position_getCapturedPiece(const struct Position* const pos);
void Position_ChangeSide(struct Position* const pos);

extern Bitboard SQUARES_BETWEEN_BB[64][64];

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

// castling rights update constants
extern const int castling_rights[64];

// convert squares to coordinates
extern const char* const square_to_coordinates[64];

void ResetBoard(struct Position* pos);
void ResetInfo(struct SearchInfo* info);
ZobristKey GeneratePosKey(const struct Position* const pos);
ZobristKey GeneratePawnKey(const struct Position* pos);
ZobristKey GenerateNonPawnKey(const struct Position* pos, int side);

Bitboard RayBetween(int square1, int square2);
void UpdatePinsAndCheckers(struct Position* pos, const int side);

// Get on what square of the board the king of color c resides
int KingSQ(const struct Position* pos, const int c);

// Parse a string of moves in coordinate format and plays them
void parse_moves(const char* moves, struct Position* pos);

// Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
Bitboard GetPieceBB(const struct Position* pos, const int piecetype);

// parse FEN string
void ParseFen(const char* command, struct Position* pos);

// Return a piece based on the type and the color
int GetPiece(const int piecetype, const int color);

// Returns the piece_type of a piece
int GetPieceType(const int piece);

// Returns true if side has at least one piece on the board that isn't a pawn, false otherwise
bool BoardHasNonPawns(const struct Position* pos, const int side);

ZobristKey keyAfter(const struct Position* pos, const Move move);

void saveBoardState(struct Position* pos);

void restorePreviousBoardState(struct Position* pos);

// if we don't have enough material to mate consider the position a draw
bool MaterialDraw(const struct Position* pos);

int ScaleMaterial(const struct Position* pos, int eval);
int EvalPositionRaw(struct Position* pos);

// position evaluation
int EvalPosition(struct Position* pos);

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
void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode);

// Fuction that returns the history bonus
int history_bonus(const int depth);

// Getters for the history heuristics
int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move);
// Clean all the history tables
void CleanHistories(struct SearchData* sd);
// Updates history heuristics for a single move
void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);
void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);

// Corrhist stuff
void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff);
int  adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const int rawEval);

extern const Bitboard file_bbs[8];
extern const Bitboard rank_bbs[8];
extern const Bitboard diagonal_bbs[15];
extern const Bitboard antidiagonal_bbs[15];

Bitboard ReverseBits(Bitboard bitboard);
Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask);
// get bishop attacks
Bitboard GetBishopAttacks(int square, Bitboard occupancy);
// get rook attacks
Bitboard GetRookAttacks(int square, Bitboard occupancy);
// get queen attacks
Bitboard GetQueenAttacks(const int square, Bitboard occupancy);

struct ThreadData;

extern Bitboard PieceKeys[12][64];
extern Bitboard enpassant_keys[64];
extern Bitboard SideKey;
extern Bitboard CastleKeys[16];

void InitNewGame(struct ThreadData* td);

// init slider piece's attack tables
void InitAttackTables();

void InitAll();

// print move (for UCI purposes)
void PrintMove(const Move move);

void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos);

void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos);

void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square);

void MakeMove(const bool UPDATE, const Move move, struct Position* pos);
// Reverts the previously played move
void UnmakeMove(const Move move, struct Position* pos);
// makes a null move (a move that doesn't move any piece)
void MakeNullMove(struct Position* pos);
// Reverts the previously played null move
void TakeNullMove(struct Position* pos);

bool next_token(const char* str, int* index, char* token);

enum MovegenType {
    MOVEGEN_NOISY = 0b01,
    MOVEGEN_QUIET = 0b10,
    MOVEGEN_ALL = 0b11
};

// is the square given in input attacked by the current given side
bool IsSquareAttacked(const struct Position* pos, const int square, const int side);

// function that adds a (not yet scored) move to a move list
void AddMoveNonScored(const Move move, struct MoveList* list);

// function that adds an (already-scored) move to a move list
void AddMoveScored(const Move move, const int score, struct MoveList* list);

// Check for move pseudo-legality
bool IsPseudoLegal(const struct Position* pos, const Move move);

// Check for move legality
bool IsLegal(struct Position* pos, const Move move);

// generate moves
void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type);

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

void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode);
Move NextMove(struct Movepicker* mp, const bool skip);

// convert ASCII character pieces to encoded constants
int char_pieces(const char ch);

// Map promoted piece to the corresponding ASCII character
char promoted_pieces(const int piece);

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

extern Move return_bestmove;

void init_thread_data(struct ThreadData* td);

// ClearForSearch handles the cleaning of the thread data from a clean state
void ClearForSearch(struct ThreadData* td);

// Starts the search process, this is ideally the point where you can start a multithreaded search
void RootSearch(int depth, struct ThreadData* td);

// SearchPosition is the actual function that handles the search, it sets up the variables needed for the search , calls the Negamax function and handles the console output
void SearchPosition(int start_depth, int final_depth, struct ThreadData* td);

// Sets up aspiration windows and starts a Negamax search
int AspirationWindowSearch(int prev_eval, int depth, struct ThreadData* td);

int Negamax(int alpha, int beta, int depth, const bool cutNode, struct ThreadData* td, struct SearchStack* ss);

int Quiescence(int alpha, int beta, struct ThreadData* td, struct SearchStack* ss);

// inspired by the Weiss engine
bool SEE(const struct Position* pos, const int move, const int threshold);

// Checks if the current position is a draw
bool IsDraw(struct Position* pos);

bool IsRepetition(const struct Position* pos); // remove this later, make static
bool Is50MrDraw(struct Position* pos); // remove this later, make static

void Optimum(struct SearchInfo* info, int time, int inc);
bool StopEarly(const struct SearchInfo* info);
bool TimeOver(const struct SearchInfo* info);

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

extern struct TTable TT;

static const uint8_t MAX_AGE = 1 << 5; // must be power of 2
static const uint8_t AGE_MASK = MAX_AGE - 1;

void* AlignedMalloc(size_t size, size_t alignment);

void AlignedFree(void* src);

void TTEntry_init(struct TTEntry* const tte);
void TTBucket_init(struct TTBucket* const ttb);

void ClearTT();
// Initialize an TT of size MB
void InitTT(uint64_t MB);

bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte);

void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, int eval, const int bound, const int depth, const bool pv, const bool wasPV);

uint64_t Index(const ZobristKey posKey);

void TTPrefetch(const ZobristKey posKey);

int ScoreToTT(int score, int ply);

int ScoreFromTT(int score, int ply);

PackedMove MoveToTT(Move move);

Move MoveFromTT(struct Position* pos, PackedMove packed_move);

uint8_t BoundFromTT(uint8_t ageBoundPV);

bool FormerPV(uint8_t ageBoundPV);

uint8_t AgeFromTT(uint8_t ageBoundPV);

uint8_t PackToTT(uint8_t bound, bool wasPV, uint8_t age);

void UpdateTableAge();