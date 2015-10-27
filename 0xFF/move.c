#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "board.h"
#include "castle.h"
#include "colour.h"
#include "move.h"
#include "piece.h"
#include "search.h"
#include "types.h"
#include "util.h"

void gen_all_moves(board_t * board, move_t * list, int * size){
	assert(*size == 0);
	assert(board->turn == 0 || board->turn == 1);
	assert(board->piece_counts[board->turn] != 0);
	assert(PIECE_IS_KING(board->squares[board->piece_locations[board->turn][0]]));
	assert(list != NULL);
	
	unsigned int to, from, cap, flag;
	int turn = board->turn;
	int direction = board->turn == ColourWhite ? -16 : 16;
	int r, rights = board->castle_rights;
	int * location = (board->piece_locations[turn]);
	
	while (*location != -1){
		from = *location;
		switch(PIECE_TYPE(board->squares[from])){
			
			case KnightFlag:
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-33]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-31]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-18]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-14]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+14]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+18]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+31]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+33]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				break;
			
			case BishopFlag:
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-17]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-15]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+15]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+17]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				break;
				
			case RookFlag:
				r = 0;
				if (KING_HAS_RIGHTS(turn,rights) && IS_RIGHT_ROOK(turn,from))
					r |= CREATE_KING_RIGHTS(turn);
				if (QUEEN_HAS_RIGHTS(turn,rights) && IS_LEFT_ROOK(turn,from))
					r |= CREATE_QUEEN_RIGHTS(turn);
				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-16]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,r);
					else break;
				}	
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-1]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,r);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+1]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,r);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+16]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,r);
					else break;
				}
				break;
				
			case QueenFlag:
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-17]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-15]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+15]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+17]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-16]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to-1]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+1]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				to = from; cap = Empty;
				while(cap == Empty){
					if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=to+16]),turn))
						list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
					else break;
				}				
				break;
				
			case KingFlag:
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-17]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-16]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-15]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from-1]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+1]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+15]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+16]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);				
				if (IS_EMPTY_OR_ENEMY((cap = board->squares[to=from+17]),turn))
					list[(*size)++] = MAKE_NORMAL_MOVE(from,to,cap,0);
				break;
			
			default: assert("Error in board_locations[][]" == 0);
		}
		
		location++;
	}
	
	location = &(board->pawn_locations[turn][0]);
	
	while (*location != -1){
		from = *location;
		int promo = 0;
		
		if (turn == ColourWhite && from / 16 == 5)
			promo = 1;
		else if(turn == ColourBlack && from / 16 == 10)
			promo = 1;
		
		to = from + direction;
		if (IS_EMPTY(board->squares[to])){
			
			if (promo) 
				for(flag = PromoteQueenFlag; flag >= PromoteKnightFlag; flag = flag >> 1)
					list[(*size)++] = MAKE_PROMOTION_MOVE(from,to,Empty,flag);
				
			else
				list[(*size)++] = MAKE_NORMAL_MOVE(from,to,Empty,0);
			
			to = to + direction;
			if (IS_EMPTY(board->squares[to]) && ((from/16) + (5 * (turn)) == 10))
				list[(*size)++] = MAKE_NORMAL_MOVE(from,to,Empty,0);
			
		}
		to = from + direction;
		if (IS_PIECE(board->squares[to+1]) && PIECE_COLOUR(board->squares[to+1])){
			if (promo)
				for(flag = PromoteQueenFlag; flag >= PromoteKnightFlag; flag = flag >> 1)
					list[(*size)++] = MAKE_PROMOTION_MOVE(from,to+1,board->squares[to+1],flag);
			else
				list[(*size)++] = MAKE_NORMAL_MOVE(from,to+1,board->squares[to+1],0);
		}
		
		if (IS_PIECE(board->squares[to-1]) && PIECE_COLOUR(board->squares[to-1])){
			if (promo)
				for(flag = PromoteQueenFlag; flag >= PromoteKnightFlag; flag = flag >> 1)
					list[(*size)++] = MAKE_PROMOTION_MOVE(from,to-1,board->squares[to-1],flag);
			else
				list[(*size)++] = MAKE_NORMAL_MOVE(from,to-1,board->squares[to-1],0);
		}
		
		if (board->ep_square == from-1 || board->ep_square == from+1)
			list[(*size)++] = MAKE_ENPASS_MOVE(from,board->ep_square+direction,board->ep_square);
		
		
		location++;
	}
}

//	bits	00-07: From Square
//	bits	08-15: To Square 
//	bits	16-23: Capture Type
//	bits	24-27: Move Type
//	bits	28-31: Castle Changes

void apply_move(board_t * board, move_t move){
	int turn = board->turn;
	int from = MOVE_GET_FROM(move);
	int to = MOVE_GET_TO(move);
	int cap = MOVE_GET_CAPTURE(move);
	
	if (MOVE_IS_NORMAL(move)){
		if (cap != Empty)
			remove_position(board,to);
		board->squares[to] = board->squares[from];
		board->positions[to] = board->positions[from];
		board->squares[from] = Empty;
		board->positions[from] = -1;
		board->castle_rights ^= MOVE_GET_CASTLE_FLAGS(move);
		board->turn = !board->turn;
	} else if (MOVE_IS_ENPASS(move)){
		board->squares[to] = board->squares[from];
		board->positions[to] = board->positions[from];
		board->squares[from] = Empty;
		board->positions[from] = -1;
		int enpass = MOVE_GET_ENPASS_SQUARE(move);
		board->squares[enpass] = Empty;
		remove_position(board,enpass);
		board->turn = !board->turn;
	}
}

void revert_move(board_t * board, move_t move){
	int from = MOVE_GET_FROM(move);
	int to = MOVE_GET_TO(move);
	int cap = MOVE_GET_CAPTURE(move);
	
	if (MOVE_IS_NORMAL(move)){
		board->squares[from] = board->squares[to];
		board->positions[from] = board->positions[to];
		board->squares[to] = cap;
		board->positions[to] = -1;
		if (cap != Empty)
			add_position(board,to);
		board->castle_rights ^= MOVE_GET_CASTLE_FLAGS(move);
		board->turn = !board->turn;
	} else if (MOVE_IS_ENPASS(move)){
		board->squares[from] = board->squares[to];
		board->positions[from] = board->squares[to];
		board->squares[to] = Empty;
		board->positions[to] = -1;
		int enpass = MOVE_GET_ENPASS_SQUARE(move);
		board->squares[enpass] = PawnFlag | board->turn;
		add_position(board,enpass);
		board->turn = !board->turn;
	}
}

void add_position(board_t * board, int to){
	int i, turn = board->turn;
	int cap = board->squares[to];
	
	if (PIECE_IS_PAWN(cap)){
		board->pawn_counts[turn] += 1;
		board->pawn_locations[turn][board->pawn_counts[turn]] = to;
		board->positions[to] = board->pawn_counts[turn];
	} else {
		board->piece_counts[turn] += 1;
		board->piece_locations[turn][board->piece_counts[turn]] = to;
		board->positions[to] = board->piece_counts[turn];
	}
}

void remove_position(board_t * board, int to){
	int i;
	int turn = board->turn;
	int cap = board->squares[to];
	
	if (PIECE_IS_PAWN(cap)){
		for(i = board->positions[to]; i < board->pawn_counts[!turn]; i++){
			board->positions[i] = board->positions[i+1];
			board->pawn_locations[!turn][i] = board->pawn_locations[!turn][i+1];
		}
	} else {
		for(i = board->positions[to]; i < board->piece_counts[!turn]; i++){
			board->positions[i] = board->positions[i+1];
			board->piece_locations[!turn][i] = board->piece_locations[!turn][i+1];
		}
	}
}