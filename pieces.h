#ifndef _PIECES_H_
#define _PIECES_H_

#define PIECE_WIDTH 128
#define PIECE_HEIGHT 128

// y coord
#define Y_WHITE 0
#define Y_BLACK PIECE_HEIGHT

#define X_PAWN   0
#define X_KNIGHT X_PAWN   + PIECE_WIDTH
#define X_BISHOP X_KNIGHT + PIECE_WIDTH 
#define X_ROOK   X_BISHOP + PIECE_WIDTH
#define X_QUEEN  X_ROOK   + PIECE_WIDTH
#define X_KING   X_QUEEN  + PIECE_WIDTH

enum PIECE {
	NONE   = -1,
	KING   = X_KING,
	QUEEN  = X_QUEEN,
	BISHOP = X_BISHOP,
	KNIGHT = X_KNIGHT,
	ROOK   = X_ROOK,
	PAWN   = X_PAWN
};

enum COLOR {
	BLACK = Y_WHITE,
	WHITE = Y_BLACK
};

struct Piece {
	int x;
	int y;

	enum PIECE name;
	enum COLOR color;
};

static inline enum PIECE starting_piece(int x, int y);
static inline void init_pieces(struct Piece *board);

enum PIECE
starting_piece(int x, int y)
{
	if (y > 1 && y < 6)
		return NONE;
	if (y == 1 || y == 6)
		return PAWN;
	if (x == 0 || x == 7)
		return ROOK;
	if (x == 1 || x == 6)
		return KNIGHT;
	if (x == 2 || x == 5)
		return BISHOP;

	return x == 3 ? KING : QUEEN;

	fprintf(stderr, "problem in starting_piece(), this should be unreachable\n");
	return NONE;
}

void
init_pieces(struct Piece *board)
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[8*y + x].x = x;
			board[8*y + x].y = y;
			board[8*y + x].name =
				starting_piece(x, y);
			board[8*y + x].color =
				y < 2 ? WHITE : BLACK;
		}
	}
}

#endif // _PIECES_H_
