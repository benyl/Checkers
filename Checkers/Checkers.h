/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*------------------------------------------------------------------------
   	Checkers.H -- Project: Checkers Game

	Polytechnic Institute of New York University
	Spring 2012 . CS 6613 : Artificial Intelligence
    Instructor: E. K. Wong
	Student: Yanbin Luo <yluo04@students.poly.edu>
 -----------------------------------------------------------------------*/

#pragma once

#include "resource.h"

#define EMPTY 0
#define WHITE 1
#define BLACK 2
#define STEP 20
#define LENGTH 75
#define MAXACT 25
#define MAXVALUE 100
#define MINVALUE -100

// ===========================================================
// Variables
// ===========================================================

int maxDepth = 15;	//max depth to search in alpha-beta pruning
int treeDepth = 0;	//max depth of the tree
int genNode = 0;	//total node generated
int pruneMax = 0;	//number of times pruning in the MAX-VALUE function
int pruneMin = 0;	//number of times pruning in the MIN-VALUE function

short initboard[8][8] = { 
	{0, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{2, 0, 2, 0, 2, 0, 2, 0},
	{0, 2, 0, 2, 0, 2, 0, 2},
	{2, 0, 2, 0, 2, 0, 2, 0}
}; //0 = empty, 1 = white, 2 = black*/
/*
const short initboard[8][8] = { 
	{0, 0, 0, 0, 0, 0, 0, 0}, // 0
	{0, 0, 0, 0, 0, 0, 0, 0}, // 1
	{0, 0, 0, 1, 0, 0, 0, 0}, // 2
	{0, 0, 0, 0, 1, 0, 0, 0}, // 3
	{0, 0, 0, 0, 0, 0, 0, 0}, // 4
	{2, 0, 0, 0, 0, 0, 2, 0}, // 5
	{0, 0, 0, 0, 0, 0, 0, 0}, // 6
	{0, 0, 0, 0, 0, 0, 0, 0}, // 7
}; //0 = empty, 1 = white, 2 = black*/

// ===========================================================
// Structures
// ===========================================================

// main system states
enum State{Start, MovePick, MoveDrop, JumpPick, JumpDrop, MultiJump, Animate, GameStop};

// piece action type
enum Action{MOVE, JUMP, PASS, STOP};

struct Piece{	// piece location
	short x;	// x axis
	short y;	// y axis
};

struct PieceAct{					// piece action
	short x_pick; short y_pick;		// source position
	short x_drop; short y_drop;		// move / jump destination
	short x_jump2; short y_jump2;	// 2nd jump destination
	short x_jump3; short y_jump3;	// 3rd jump destination
};

struct BoardInfo{
	short board[8][8];		// board information
	short mapping[8][8];	// mapping between board[8][8] and bw[2][12]
	Piece bw[2][12];		// piece location, bw[0] = BLACK, bw[1] = WHITE
	short bw_num[2];		// piece number, bw_num[0] = BLACK, bw_num[1] = WHITE
	short role;				// action role, 2 = BLACK, 1 = WHITE
};

enum State NextState(short x, short y, enum State curstate, short (*board)[8], short *role, PieceAct *rc);
enum Action NextAction(short (*board)[8], short actrole, PieceAct *rc);
enum Action GetActionList(BoardInfo *info, PieceAct *actlist);
enum Action GetMultiJumpList(BoardInfo *info, PieceAct *actlist);
void GetActResult(BoardInfo *info, PieceAct act);
short MaxValue(BoardInfo *info, PieceAct act, short alpha, short beta, short depth, PieceAct *rc);
short MinValue(BoardInfo *info, PieceAct act, short alpha, short beta, short depth, PieceAct *rc);


//=========================================================
// Function Name: Next State
//
// Parameter: mouse click location (x, y), current state(curstate)
// current board(board), current role(role), return action(rc)
//
// Output: enum State - the next state of the system
//
// Description: input the board information, 
//				return the next state of the system
//
//=========================================================

enum State NextState(short x, short y, enum State curstate, short (*board)[8], short *role, PieceAct *rc)
{	
	enum State state = curstate;
	enum Action nextact = STOP;
	static enum Action lastact = STOP;

	switch (state)
	{
	case Start:
		if (*role == BLACK)
		{
			nextact = NextAction(board, *role, NULL);
			if (lastact == PASS && nextact == PASS) nextact = STOP;
			lastact = nextact; //store the last action

			if (nextact == JUMP) state = JumpPick;
			else if (nextact == MOVE) state = MovePick;
			else if (nextact == STOP) state = GameStop;
			else // PASS
			{
				*role = (*role==BLACK)?WHITE:BLACK; //switch role
				return NextState(0,0,state,board,role,rc);
			}
		}
		else // role == WHITE
		{
			nextact = NextAction(board, *role, rc);
			if (lastact == PASS && nextact == PASS) nextact = STOP;
			lastact = nextact; //store the last action

			if (nextact == JUMP || nextact == MOVE)
			{
				board[rc->x_pick][rc->y_pick]=EMPTY;
				state = Animate;
			}
			else if (nextact == STOP) state = GameStop;
			else //PASS
			{
				*role = (*role==BLACK)?WHITE:BLACK; //switch role
				return NextState(0,0,state,board,role,rc);
			}
		}
		break;

	case MovePick: // not pick, pick pieces (user only pick black)
		if (board[x][y]==BLACK){
			rc->x_pick = x;
			rc->y_pick = y;

			state = MoveDrop;
		}
		break;
	case MoveDrop: // already pick, drop
		// drop at the same place, cancel pick pieces
		if (rc->x_pick==x && rc->y_pick==y){
			rc->x_pick = -1;
			rc->y_pick = -1;

			state = MovePick;
		}
		// drop at the same place, cancel pick pieces
		else if((rc->x_pick - x == 1) &&
			    (abs(rc->y_pick - y) == 1) &&
			    (board[x][y]==EMPTY))
		{
			rc->x_drop = x;
			rc->y_drop = y;
			board[rc->x_pick][rc->y_pick]=EMPTY;

			state = Animate;
		}
		break;

	case JumpPick: // not pick, pick pieces (user only pick black)
		if (board[x][y]==BLACK){
			rc->x_pick = x;
			rc->y_pick = y;

			state = JumpDrop;
		}
		break;

	case JumpDrop:
		// drop at the same place, cancel pick pieces
		if (rc->x_pick==x && rc->y_pick==y){
			rc->x_pick = -1;
			rc->y_pick = -1;

			state = JumpPick;
		}
		// valid jump
		else if((rc->x_pick - x == 2) &&
			    (abs(rc->y_pick - y) == 2) &&
				(board[(rc->x_pick + x)/2][(rc->y_pick + y)/2]==WHITE) &&
			    (board[x][y]==EMPTY))
		{
			rc->x_drop = x;
			rc->y_drop = y;
			board[rc->x_pick][rc->y_pick]=EMPTY;

			state = Animate;
		}
		break;
		
	case MultiJump:
		// valid jump
		if((rc->x_pick - x == 2) &&
			(abs(rc->y_pick - y) == 2) &&
			(board[(rc->x_pick + x)/2][(rc->y_pick + y)/2]==WHITE) &&
			(board[x][y]==EMPTY))
		{
			rc->x_drop = x;
			rc->y_drop = y;
			board[rc->x_pick][rc->y_pick]=EMPTY;

			state = Animate;
		}
		break;

	case Animate:
		// after animate, update the board
		if (rc->x_pick != rc->x_drop) // is a move or jump
		{
			board[rc->x_drop][rc->y_drop] = *role;
			
			if (abs(rc->x_pick - rc->x_drop) == 2) // if last act is jump
			{
				board[(rc->x_pick + rc->x_drop)/2][(rc->y_pick + rc->y_drop)/2]=EMPTY;

				if(*role == BLACK)
				{
					//if multiple jump
					if((rc->x_drop>1 && rc->y_drop>1 && //jump left
						board[rc->x_drop - 1][rc->y_drop - 1] == WHITE &&
						board[rc->x_drop - 2][rc->y_drop - 2] == EMPTY) ||
						(rc->x_drop>1 && rc->y_drop<6 && //jump right
						board[rc->x_drop - 1][rc->y_drop + 1] == WHITE &&
						board[rc->x_drop - 2][rc->y_drop + 2] == EMPTY))
					{
						rc->x_pick = rc->x_drop; 
						rc->y_pick = rc->y_drop; 
						rc->x_drop = -1;
						rc->y_drop = -1;

						state = MultiJump;
						break;
					}
				}
				else //WHITE
				{
					rc->x_pick = rc->x_drop;
					rc->y_pick = rc->y_drop;
					rc->x_drop = rc->x_jump2;
					rc->y_drop = rc->y_jump2;
					rc->x_jump2 = rc->x_jump3;
					rc->y_jump2 = rc->y_jump3;
					rc->x_jump3 = -1;
					rc->y_jump3 = -1;
					
					if(rc->x_drop != -1) //WHITE multijump
					{
						board[rc->x_pick][rc->y_pick] = EMPTY;
						state = Animate;
						break;
					}
				}
			}

			rc->x_pick = -1; rc->y_pick = -1; rc->x_drop = -1; rc->y_drop = -1;	
			rc->x_jump2 = -1; rc->y_jump2 = -1; rc->x_jump3 = -1; rc->y_jump3 = -1;
		}
		
		*role = (*role==BLACK)?WHITE:BLACK; //switch role
		state = Start;
		return NextState(0,0,state,board,role,rc);
		break;

	default:
		break;
	}

	return state;
}


//=========================================================
// Function Name: NextAction
// 
// Parameter: current board (curboard), current role, return action (rc)
//
// Output: enum Action - the next action type
//
// Description: determine the next availeble action, for White side, 
//				use alpha-beta pruning to calculate the next best action
//
//=========================================================

enum Action NextAction(short (*curboard)[8], short role, PieceAct *rc)
{
	BoardInfo info;
	PieceAct actlist[MAXACT]={0};
	enum Action nextact = PASS;
	
	memset(&info, 0, sizeof(BoardInfo));
	info.role = role;

	for(short i=0;i<8;i++)
	for(short j=0;j<8;j++)
		if (curboard[i][j] == BLACK)
		{
			info.board[i][j] = BLACK;
			info.bw[0][info.bw_num[0]].x = i;
			info.bw[0][info.bw_num[0]].y = j;
			info.mapping[i][j] = info.bw_num[0];
			info.bw_num[0] ++;
		}
		else if (curboard[i][j] == WHITE)
		{
			info.board[i][j] = WHITE;
			info.bw[1][info.bw_num[1]].x = i;
			info.bw[1][info.bw_num[1]].y = j;
			info.mapping[i][j] = info.bw_num[1];
			info.bw_num[1] ++;
		}
		else
		{
			info.board[i][j] = EMPTY;
			info.mapping[i][j] = - 1;
		}

	if (info.bw_num[0] == 0 || info.bw_num[1] == 0) return STOP;

	nextact = GetActionList(&info,actlist);

	if (rc != NULL)
	{
		//initialize the counter
		treeDepth = 0;
		genNode = 0;
		pruneMax = 0;
		pruneMin = 0;

		PieceAct best={ -1, -1, -1, -1};
		MaxValue(&info, best, MINVALUE - 1, MAXVALUE + 1, 0, rc);
	}

	return nextact;
}

//=========================================================
// Function Name: GetActionList
// 
// Parameter: board infomation (info), return action list (actlist)
//
// Output: enum Action - the next available action type
//
// Description: get a list of all the availeble action
//
//=========================================================

enum Action GetActionList(BoardInfo *info, PieceAct *actlist)
{	
	short mid = 0; //move list index
	short jid = 0; //jump list index
	PieceAct jumplist[MAXACT] = {0};
	PieceAct movelist[MAXACT] = {0};
	Piece *self = info->bw[info->role %2]; //BLACK = bw[0]; WHITE = bw[1];

	if (info->role == BLACK)
		for(short i = 0; i<info->bw_num[0]; i++)
		{
			if( //jump left
				self[i].x>1 && self[i].y>1 &&
				info->board[self[i].x - 1][self[i].y - 1] == WHITE &&
				info->board[self[i].x - 2][self[i].y - 2] == EMPTY)
			{
				jumplist[jid].x_pick = self[i].x;
				jumplist[jid].y_pick = self[i].y;
				jumplist[jid].x_drop = self[i].x - 2;
				jumplist[jid].y_drop = self[i].y - 2;
				jid ++;
			}
			if( //jump right
				self[i].x>1 && self[i].y<6 &&
				info->board[self[i].x - 1][self[i].y + 1] == WHITE &&
				info->board[self[i].x - 2][self[i].y + 2] == EMPTY)
			{
				jumplist[jid].x_pick = self[i].x;
				jumplist[jid].y_pick = self[i].y;
				jumplist[jid].x_drop = self[i].x - 2;
				jumplist[jid].y_drop = self[i].y + 2;
				jid ++;
			}
			if( //move left
				self[i].x>0 && self[i].y>0 &&
				info->board[self[i].x - 1][self[i].y - 1] == EMPTY)
			{
				movelist[mid].x_pick = self[i].x;
				movelist[mid].y_pick = self[i].y;
				movelist[mid].x_drop = self[i].x - 1;
				movelist[mid].y_drop = self[i].y - 1;
				mid ++;
			}
			if( //move right
				self[i].x>0 && self[i].y<7 &&
				info->board[self[i].x - 1][self[i].y + 1] == EMPTY)
			{
				movelist[mid].x_pick = self[i].x;
				movelist[mid].y_pick = self[i].y;
				movelist[mid].x_drop = self[i].x - 1;
				movelist[mid].y_drop = self[i].y + 1;
				mid ++;
			}
		}
	else
		for(short i = 0; i<info->bw_num[1]; i++)
		{
			if( //jump left
				self[i].x<6 && self[i].y>1 &&
				info->board[self[i].x + 1][self[i].y - 1] == BLACK &&
				info->board[self[i].x + 2][self[i].y - 2] == EMPTY)
			{
				jumplist[jid].x_pick = self[i].x;
				jumplist[jid].y_pick = self[i].y;
				jumplist[jid].x_drop = self[i].x + 2;
				jumplist[jid].y_drop = self[i].y - 2;
				jid ++;
			}
			if( //jump right
				self[i].x<6 && self[i].y<6 &&
				info->board[self[i].x + 1][self[i].y + 1] == BLACK &&
				info->board[self[i].x + 2][self[i].y + 2] == EMPTY)
			{
				jumplist[jid].x_pick = self[i].x;
				jumplist[jid].y_pick = self[i].y;
				jumplist[jid].x_drop = self[i].x + 2;
				jumplist[jid].y_drop = self[i].y + 2;
				jid ++;
			}
			if( //move left
				self[i].x<7 && self[i].y>0 &&
				info->board[self[i].x + 1][self[i].y - 1] == EMPTY)
			{
				movelist[mid].x_pick = self[i].x;
				movelist[mid].y_pick = self[i].y;
				movelist[mid].x_drop = self[i].x + 1;
				movelist[mid].y_drop = self[i].y - 1;
				mid ++;
			}
			if( //move right
				self[i].x<7 && self[i].y<7 &&
				info->board[self[i].x + 1][self[i].y + 1] == EMPTY)
			{
				movelist[mid].x_pick = self[i].x;
				movelist[mid].y_pick = self[i].y;
				movelist[mid].x_drop = self[i].x + 1;
				movelist[mid].y_drop = self[i].y + 1;
				mid ++;
			}
		}
		
	if (jid >0) 
	{
		memcpy(actlist, jumplist, sizeof(jumplist));
		GetMultiJumpList(info, actlist);
		return JUMP;
	}
	if (mid >0) 
	{
		memcpy(actlist, movelist, sizeof(movelist));
		return MOVE;
	}
	return PASS;
}

//=========================================================
// Function Name: GetMultiJumpList
// 
// Parameter: board infomation (info), return action list (actlist)
//
// Output: enum Action - the next available action type
//
// Description: get a list of all the multiple jump action
//
//=========================================================
enum Action GetMultiJumpList(BoardInfo *info, PieceAct *actlist)
{	
	short jid = 0; //jump list index
	PieceAct jumplist[MAXACT] = {0};

	if (info->role == BLACK)
		for(short i = 0; i<MAXACT; i++)
		{
			short x = actlist[i].x_drop;
			short y = actlist[i].y_drop;
			bool jump2flag = false;
			bool jump3flag = false;

			if(actlist[i].x_pick == actlist[i].x_drop) break;

			if( //jump left
				x >1 && y >1 &&
				info->board[x - 1][y - 1] == WHITE &&
				info->board[x - 2][y - 2] == EMPTY)
			{
				if( //jump left
					(x - 2) > 1 && (y - 2) > 1 &&
					info->board[(x - 2) - 1][(y - 2) - 1] == WHITE &&
					info->board[(x - 2) - 2][(y - 2) - 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = (x - 2) - 2;
					jumplist[jid].y_jump3 = (y - 2) - 2;
					jid ++;
					jump3flag = true;
				}
				if( //jump right
					(x - 2) > 1 && (y - 2) < 6 &&
					info->board[(x - 2) - 1][(y - 2) + 1] == WHITE &&
					info->board[(x - 2) - 2][(y - 2) + 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = (x - 2) - 2;
					jumplist[jid].y_jump3 = (y - 2) + 2;
					jid ++;
					jump3flag = true;
				}
				if (jump3flag != true)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = -1;
					jumplist[jid].y_jump3 = -1;
					jid ++;
				}
				jump2flag = true;
			}
			if( //jump right
				x>1 && y<6 &&
				info->board[x - 1][y + 1] == WHITE &&
				info->board[x - 2][y + 2] == EMPTY)
			{
				if( //jump left
					(x - 2) > 1 && (y + 2) > 1 &&
					info->board[(x - 2) - 1][(y + 2) - 1] == WHITE &&
					info->board[(x - 2) - 2][(y + 2) - 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = (x - 2) - 2;
					jumplist[jid].y_jump3 = (y + 2) - 2;
					jid ++;
					jump3flag = true;
				}
				if( //jump right
					(x - 2) > 1 && (y + 2) < 6 &&
					info->board[(x - 2) - 1][(y + 2) + 1] == WHITE &&
					info->board[(x - 2) - 2][(y + 2) + 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = (x - 2) - 2;
					jumplist[jid].y_jump3 = (y + 2) + 2;
					jid ++;
					jump3flag = true;
				}
				if (jump3flag != true)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x - 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = -1;
					jumplist[jid].y_jump3 = -1;
					jid ++;
				}
				jump2flag = true;
			}
			
			if (jump2flag != true)
			{
				jumplist[jid] = actlist[i];
				jumplist[jid].x_jump2 = -1;
				jumplist[jid].y_jump2 = -1;
				jumplist[jid].x_jump3 = -1;
				jumplist[jid].y_jump3 = -1;
				jid ++;
			}
		}
	else
		for(short i = 0; i<MAXACT; i++)
		{
			short x = actlist[i].x_drop;
			short y = actlist[i].y_drop;
			bool jump2flag = false;
			bool jump3flag = false;

			if(actlist[i].x_pick == actlist[i].x_drop) break;

			if( //jump left
				x < 6 && y >1 &&
				info->board[x + 1][y - 1] == BLACK &&
				info->board[x + 2][y - 2] == EMPTY)
			{
				if( //jump left
					(x + 2) < 6 && (y - 2) > 1 &&
					info->board[(x + 2) + 1][(y - 2) - 1] == BLACK &&
					info->board[(x + 2) + 2][(y - 2) - 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = (x + 2) + 2;
					jumplist[jid].y_jump3 = (y - 2) - 2;
					jid ++;
					jump3flag = true;
				}
				if( //jump right
					(x + 2) < 6 && (y - 2) < 6 &&
					info->board[(x + 2) + 1][(y - 2) + 1] == BLACK &&
					info->board[(x + 2) + 2][(y - 2) + 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = (x + 2) + 2;
					jumplist[jid].y_jump3 = (y - 2) + 2;
					jid ++;
					jump3flag = true;
				}
				if (jump3flag != true)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y - 2;
					jumplist[jid].x_jump3 = -1;
					jumplist[jid].y_jump3 = -1;
					jid ++;
				}
				jump2flag = true;
			}
			if( //jump right
				x<6 && y<6 &&
				info->board[x + 1][y + 1] == BLACK &&
				info->board[x + 2][y + 2] == EMPTY)
			{
				if( //jump left
					(x + 2) < 6 && (y + 2) > 1 &&
					info->board[(x + 2) + 1][(y + 2) - 1] == BLACK &&
					info->board[(x + 2) + 2][(y + 2) - 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = (x + 2) + 2;
					jumplist[jid].y_jump3 = (y + 2) - 2;
					jid ++;
					jump3flag = true;
				}
				if( //jump right
					(x + 2) < 6 && (y - 2) < 6 &&
					info->board[(x + 2) + 1][(y + 2) + 1] == BLACK &&
					info->board[(x + 2) + 2][(y + 2) + 2] == EMPTY)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = (x + 2) + 2;
					jumplist[jid].y_jump3 = (y + 2) + 2;
					jid ++;
					jump3flag = true;
				}
				if (jump3flag != true)
				{
					jumplist[jid] = actlist[i];
					jumplist[jid].x_jump2 = x + 2;
					jumplist[jid].y_jump2 = y + 2;
					jumplist[jid].x_jump3 = -1;
					jumplist[jid].y_jump3 = -1;
					jid ++;
				}
				jump2flag = true;
			}
			
			if (jump2flag != true)
			{
				jumplist[jid] = actlist[i];
				jumplist[jid].x_jump2 = -1;
				jumplist[jid].y_jump2 = -1;
				jumplist[jid].x_jump3 = -1;
				jumplist[jid].y_jump3 = -1;
				jid ++;
			}
		}

	memcpy(actlist, jumplist, sizeof(jumplist));
	return JUMP;
}


//=========================================================
// Function Name: GetActResult
// 
// Parameter: board infomation (info), the action to take (act)
//
// Output: void
//
// Description: get the result of given action
//
//=========================================================
void GetActResult(BoardInfo *info, PieceAct act)
{
	short id, x_mid, y_mid;
	Piece *self, *enemy;
	short *self_num, *enemy_num;

	if(abs(act.x_pick - act.x_drop) == 2) //a jump
	{
		//move the piece
		id = info->mapping[act.x_pick][act.y_pick];
		info->mapping[act.x_drop][act.y_drop] = id;
		info->mapping[act.x_pick][act.y_pick] = -1;

		info->board[act.x_drop][act.y_drop] = info->role;
		info->board[act.x_pick][act.y_pick] = EMPTY;

		self = info->bw[info->role %2]; //BLACK = bw[0]; WHITE = bw[1];
		self[id].x = act.x_drop;
		self[id].y = act.y_drop;

		// delete the middle piece
		x_mid = (act.x_pick + act.x_drop) / 2;
		y_mid = (act.y_pick + act.y_drop) / 2;
		id = info->mapping[x_mid][y_mid];

		enemy = info->bw[(info->role + 1) %2];
		enemy_num = &(info->bw_num[(info->role + 1) %2]);

		// move the deleted piece to end of array
		if(id != *enemy_num - 1)
		{
			info->mapping[x_mid][y_mid] = *enemy_num - 1;
			info->mapping[enemy[*enemy_num - 1].x][enemy[*enemy_num - 1].y]
				= id;

			enemy[id].x = enemy[*enemy_num - 1].x;
			enemy[id].y = enemy[*enemy_num - 1].y;
			
			enemy[*enemy_num - 1].x = x_mid;
			enemy[*enemy_num - 1].y = y_mid;
		}

		x_mid = enemy[*enemy_num - 1].x;
		y_mid = enemy[*enemy_num - 1].y;
		info->board[x_mid][y_mid] = EMPTY;
		info->mapping[x_mid][y_mid] = -1;

		enemy[*enemy_num - 1].x = 0;
		enemy[*enemy_num - 1].y = 0;
		
		(*enemy_num) --;

		if(act.x_jump2 != -1) //WHITE multijump
		{
			act.x_pick = act.x_drop;
			act.y_pick = act.y_drop;
			act.x_drop = act.x_jump2;
			act.y_drop = act.y_jump2;
			act.x_jump2 = act.x_jump3;
			act.y_jump2 = act.y_jump3;
			act.x_jump3 = -1;
			act.y_jump3 = -1;

			GetActResult(info, act);
		}
	}
	else if(abs(act.x_pick - act.x_drop) == 1) //a move
	{	
		//move the piece
		id = info->mapping[act.x_pick][act.y_pick];
		info->mapping[act.x_drop][act.y_drop] = id;
		info->mapping[act.x_pick][act.y_pick] = -1;

		info->board[act.x_drop][act.y_drop] = info->role;
		info->board[act.x_pick][act.y_pick] = EMPTY;

		self = info->bw[info->role %2]; //BLACK = bw[0]; WHITE = bw[1];
		self[id].x = act.x_drop;
		self[id].y = act.y_drop;
	}
	//else pass
}

//=========================================================
// Function Name: MaxValue
// 
// Parameter: board infomation(info), the action to take(act), 
//				pruning factor(alpha-beta), current depth of tree(depth),
//				return action(rc)
//
// Output: short - estimate value of the tree
//
// Description: max value function in alpha-beta pruning algorithm, 
//				return the action correspond to the estimate value in rc
//
//=========================================================
short MaxValue(BoardInfo *info, PieceAct act, short alpha, short beta, short depth, PieceAct *rc)
{
	BoardInfo curinfo;
	memcpy(&curinfo, info, sizeof(BoardInfo));
	curinfo.role = BLACK;

	if (depth > treeDepth) treeDepth = depth;

	//=======================================
	// get the result of action rc
	//=======================================
	GetActResult(&curinfo, act);

	//=======================================
	// terminal test
	//=======================================
	
	if(curinfo.bw_num[0] == 0) return MAXVALUE; //BLACK has no piece, WHITE win.
	else if(curinfo.bw_num[1] == 0) return MINVALUE; //BLACK has no piece, WHITE win.
	else if(depth >= maxDepth)
	{
		int score = 0;
		for(short i = 0; i<curinfo.bw_num[1]; i++)
			if(curinfo.bw[1][i].x == 7) score += 2;
			else score ++;
		
		for(short i = 0; i<curinfo.bw_num[0]; i++)
			if(curinfo.bw[0][i].x == 0) score -= 2;
			else score --;

		return score;
	}

	PieceAct actlist[MAXACT]={0};
	curinfo.role = WHITE;
	enum Action nextact = GetActionList(&curinfo,actlist);
	if (nextact == PASS) 
	{
		if(act.x_pick == act.x_drop) //GameStop
		{
			int score = 0;
			for(short i = 0; i<curinfo.bw_num[1]; i++)
				if(curinfo.bw[1][i].x == 7) score += 2;
				else score ++;
		
			for(short i = 0; i<curinfo.bw_num[0]; i++)
				if(curinfo.bw[0][i].x == 0) score -= 2;
				else score --;

			return score;
		}
		else //PASS
			return MinValue(&curinfo, actlist[0], alpha, beta, depth + 1, NULL); 
	}
	
	//=======================================
	// for each action call minvalue
	//=======================================
	int temp=0, score = MINVALUE - 1;

	for(int i = 0;i<MAXACT;i++)
	{
		if(actlist[i].x_pick == actlist[i].x_drop) break;
		temp = MinValue(&curinfo, actlist[i], alpha, beta, depth + 1, NULL); 
		genNode ++;
		if (temp > score)
		{
			score = temp;
			if (rc != NULL) *rc = actlist[i];
		}
		if (temp >= beta) 
		{
			pruneMax ++;
			return temp;
		}
		alpha = (temp > alpha) ? temp : alpha;
	}
	return score;
}

//=========================================================
// Function Name:
// 
// Parameter: 
//
// Description:
//
//=========================================================

short MinValue(BoardInfo *info, PieceAct act, short alpha, short beta, short depth, PieceAct *rc)
{
	BoardInfo curinfo;
	memcpy(&curinfo, info, sizeof(BoardInfo));
	curinfo.role = WHITE;

	if (depth > treeDepth) treeDepth = depth;

	//=======================================
	// get the result of action rc
	//=======================================
	GetActResult(&curinfo, act);

	//=======================================
	// terminal test
	//=======================================
	
	if(curinfo.bw_num[0] == 0) return MAXVALUE; //BLACK has no piece, WHITE win.
	else if(curinfo.bw_num[1] == 0) return MINVALUE; //BLACK has no piece, WHITE win.
	else if(depth >= maxDepth)
	{
		int score = 0;
		for(short i = 0; i<curinfo.bw_num[1]; i++)
			if(curinfo.bw[1][i].x == 7) score += 2;
			else score ++;
		
		for(short i = 0; i<curinfo.bw_num[0]; i++)
			if(curinfo.bw[0][i].x == 0) score -= 2;
			else score --;

		return score;
	}

	PieceAct actlist[MAXACT]={0};
	curinfo.role = BLACK;
	enum Action nextact = GetActionList(&curinfo,actlist);
	if (nextact == PASS)
	{
		if (act.x_pick == act.x_drop) //GameStop
		{
			int score = 0;
			for(short i = 0; i<curinfo.bw_num[1]; i++)
				if(curinfo.bw[1][i].x == 7) score += 2;
				else score ++;
		
			for(short i = 0; i<curinfo.bw_num[0]; i++)
				if(curinfo.bw[0][i].x == 0) score -= 2;
				else score --;

			return score;
		}
		else // PASS
			return MaxValue(&curinfo, actlist[0], alpha, beta, depth + 1, NULL); 
	}
	
	//=======================================
	// for each action call MaxValue
	//=======================================
	int temp=0,	score = MAXVALUE + 1;

	for(int i = 0;i<MAXACT;i++)
	{
		if(actlist[i].x_pick == actlist[i].x_drop) break;
		temp = MaxValue(&curinfo, actlist[i], alpha, beta, depth + 1, NULL); 
		genNode ++;
		if (temp < score)
		{
			score = temp;
			if (rc != NULL) *rc = actlist[i];
		}
		if (temp <= alpha) 
		{
			pruneMin++;
			return temp;
		}
		beta = (temp < beta) ? temp : beta;
	}
	return score;
}