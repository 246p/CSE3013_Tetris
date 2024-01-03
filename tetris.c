#include "tetris.h"


static struct sigaction act, oact;
int B, count, score_number=0;
int REC_MAX_SCORE=-100000;
int REC_MAX_Y=-1;
int rec_darw_flag=0;
time_t init,end;
long long Rec_size=0;
int main(){
	int exit=0;
	createRankList();
	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_RANK: rank(); break;
		default: break;
		}
	}
	//종료할때
	writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
	for(i=0;i<DEPTH;i++)
		nextBlock[i]=rand()&7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	// shadow init;
	shadowX=WIDTH/2-2;
	shadowR=0;
	shadowY=HEIGHT-4;

	score=0;	
	gameOver=0;
	timed_out=0;
	
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
	recommendR=0;
	recommendX=0;
	recommendY=0;
	Rec_head=malloc(sizeof(RecNode));
	Rec_head->lv=0;
	for(int i=0;i<HEIGHT;i++)
		for(int j=0;j<WIDTH;j++)
			Rec_head->f[i][j]=0;
	Rec_head->parent=NULL;
	Rec_head->rec_score=0;
	Rec_head->x=0;
	Rec_head->y=0;
	Rec_head->r=0;
	REC_MAX_SCORE=-100000;
	recommend(Rec_head);
	rec_darw_flag=1;
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	if (command != QUIT && rec_flag)
		return KEY_DOWN;
	return command;

}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			end=time(NULL);
			refresh();
			getch();
			if(!rec_flag)
				newRank(score);
			return;
		}
	}while(!gameOver);
	end=time(NULL);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	if(!rec_flag)
		newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////1주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(WIDTH<=blockX+j) return 0;
				if(0>blockX+j) return 0;
				if(HEIGHT<=blockY+i) return 0;
				if(0>blockY+i) return 0;
				if(f[blockY+i][blockX+j]) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	int prevX = blockX, prevY = blockY, prevR = blockRotate;
	switch (command)
	{
	case KEY_UP:
		prevR = (prevR + 3) % 4;
		break;
	case KEY_DOWN:
		prevY--;
		break;
	case KEY_RIGHT:
		prevX--;
		break;
	case KEY_LEFT:
		prevX++;
		break;
	}
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(block[currentBlock][prevR][i][j]&&i+prevY>=0){
					move(prevY+i+1,prevX+j+1);
					printw(".");
			}
		}
	}
	//delete shadow;
	
	//3. 새로운 블록 정보를 그린다.
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate); 

}

void BlockDown(int sig){
	// user code
	//강의자료 p26-27의 플로우차트를 참고한다.
	
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){
		blockY++;
		if(rec_flag){
			blockX=recommendX;
			blockY=recommendY;
			blockRotate=recommendR;
		}
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}
	else {
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		if(blockY==-1) gameOver = TRUE;
		else{
			for(int i=0;i<DEPTH-1;i++)
				nextBlock[i]=nextBlock[i+1];
			nextBlock[DEPTH-1]=rand()%7;
			blockX=WIDTH/2-2;
			blockY=-1;
			blockRotate=0;
			shadowR=0;
			shadowX=WIDTH/2-2;
			shadowY=-1;
			DrawNextBlock(nextBlock);
			score += DeleteLine(field);

			//recomandinit;
			recommendR = 0;
			recommendX = 0;
			recommendY = 0;
			for (int i = 0; i < HEIGHT; i++)
				for (int j = 0; j < WIDTH; j++)
					Rec_head->f[i][j] = field[i][j];
			REC_MAX_SCORE = -10000000;
			REC_MAX_Y=-1;
			modified_recommend(Rec_head);

			//
			PrintScore(score);
		}
		DrawField();
	}
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//Block이 추가된 영역의 필드값을 바꾼다.
	int touched=0;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(0 <= blockY+i && blockY+i < HEIGHT && 0 <= blockX+j && blockX+j < WIDTH){
					if(f[blockY+i+1][blockX+j]||blockY+i==HEIGHT-1) touched++;
				}
			}
		}
	}
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(0 <= blockY+i && blockY+i < HEIGHT && 0 <= blockX+j && blockX+j < WIDTH){
					f[blockY+i][blockX+j]=1;
				}
			}
		}
	}

					
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int count_line=0;
	for(int i=0;i<HEIGHT;i++){
		int flag=1;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
		for(int j=0;j<WIDTH;j++)
			if(f[i][j]==0){
				flag=0;
				break;
			}
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
		if(flag){
			count_line++;
			for(int j=i-1;j>=0;j--){
				for(int k=0;k<WIDTH;k++){
					f[j+1][k]=f[j][k];
				}
			}
		}	
	}
	return count_line*count_line*100;
}

///////////////////////////////////////////////////////////////////////////
// 1주차 과제
void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(block[blockID][shadowR][i][j]){
				move(shadowY+i+1,shadowX+j+1);
				printw(".");
			}
		}
	}
	for(int level=HEIGHT-1;level>=y;level--){
		int flag=1;
		if(!CheckToMove(field,blockID,blockRotate,level,x))
			flag=0;
		for(int l=level-1;l>y;l--){
			if(!CheckToMove(field,blockID,blockRotate,l,x))
				flag=0;
		}
		if(flag){
			shadowY=level;
			shadowX=x;
			shadowR=blockRotate;
			DrawBlock(shadowY,shadowX,blockID,shadowR,'/');
			break; 
		}
	}
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	if(rec_darw_flag)
		DrawRecommend(recommendY,recommendX,blockID,recommendR);
	if(!rec_flag)
		DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
}

/////////////////////////2주차 실습에서 구현해야 할 함수/////////////////////////

void createRankList(){
	// user code
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	head = malloc(sizeof(Node));
	tail = head;
	// 1. 파일 열기
	fp = fopen("rank.txt", "r");
	// 2. 정보읽어오기
		/* int fscanf(FILE* stream, const char* format, ...);
		stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
		format: 형식지정자 등등
		변수의 주소: 포인터
		return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
		// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if (fp != NULL){
		fscanf(fp, "%d", &score_number);
		char temp_name[NAMELEN];
		int temp_score;
		while (1){
			if (fscanf(fp, "%s %d\n", temp_name, &temp_score) != EOF){
				Node *newNode;
				newNode = malloc(sizeof(Node));
				strcpy(newNode->name, temp_name);
				newNode->score = temp_score;
				newNode->link = NULL;
				tail->link = newNode;
				tail = newNode;
			}
			else
				break;
		}
		// 4. 파일닫기
		fclose(fp);
	}
}

void rank(){
    // user code
    //목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
    //1. 문자열 초기화
    int X=1, Y=score_number, ch, i, j;
    clear();
    //2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
    //3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch=wgetch(stdscr);
    //4. 각 메뉴에 따라 입력받을 값을 변수에 저장
    //4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
    if (ch == '1'){
		echo();
		printw("X: ");
		wscanw(stdscr,"%d",&X);
		printw("Y: ");
		wscanw(stdscr,"%d",&Y);
		noecho();
		printw("      name      |  score\n");
		printw("------------------------------\n");
		if (X < 1 || Y > score_number || X > Y)
			printw("\nserach failure: no rank in the list\n");
		else{
			int c=1;
			Node *currNode = head->link;
			while (currNode){
				if (c > Y)
					break;
				if (c >= X)
					printw("%-16s|  %d\n", currNode->name, currNode->score);
				c++;
				currNode=currNode->link;
			}
		}
	}

    //4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
    else if ( ch == '2') {
        char str[NAMELEN+1];
        int check = 0;
		printw("input the name: ");
		echo();
		scanw("%s",str);
		noecho();
		Node *currNode = head->link;
		printw("      name      |  score\n");
		printw("------------------------------\n");
		while (currNode){
				if(!strcmp(str,currNode->name)){
					printw("%-16s|  %d\n", currNode->name, currNode->score);
					check++;
				}
				currNode=currNode->link;
			}
		if(!check)
			printw("seach filure: no name in the list\n");
    }

    //4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
    else if ( ch == '3') {
		int num;
		printw("input the num: ");
		echo();
		scanw("%d", &num);
		noecho();
		int c = 1;
		Node *currNode = head->link;
		Node *prevNode = head;
		if (num < 1 || num > score_number)
			printw("\nserach failure: the rank not in the list\n");
		else{
			while (currNode){
				if (num == c){
					printw("\nresult: the rank deleted\n");
					prevNode->link=currNode->link;
					free(currNode);
					score_number--;
					writeRankFile();
					break;
				}
				else{
					c++;
					prevNode = currNode;
					currNode = currNode->link;
				}
			}
		}
	}
	getch();
}

void writeRankFile(){
	
	// user code
    // 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
    int sn, i;
    //1. "rank.txt" 연다
    FILE *fp = fopen("rank.txt", "w");
    //2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp,"%d",score_number);
    //3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node* curr=head->link;
	while(curr){
		fprintf(fp,"\n%s %d",curr->name,curr->score);
		curr=curr->link;
	}
	fclose(fp);
}


void newRank(int score){
	// user code
    // 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
    char str[NAMELEN+1];
    int i, j;
    clear();
    //1. 사용자 이름을 입력받음
	echo();
	printw("your name: ");
	wscanw(stdscr,"%s",str);
	noecho();
	Node* newNode;
	newNode=malloc(sizeof(Node));
	newNode->score=score;
	strcpy(newNode->name,str);
    //2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	Node *curr = head->link;
	Node *prev=head;
	while (curr){
		if (curr->score < score)
			break;
		prev = curr;
		curr = curr->link;
	}
	newNode->link=curr;
	prev->link=newNode;
	score_number++;
	writeRankFile();
}


/////////////////////////3주차 실습에서 구현해야 할 함수/////////////////////////
void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y,x,blockID,blockRotate,'R');
}

void recommend(RecNode *root){ // tree를 만들어줌
	if (root->lv >= DEPTH)
		return;
	// 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	// user code
	for (int r = 0; r < r_state[root->lv]; r++){
		for (int x = -2; x < 13; x++){
			if(!CheckToMove(root->f,nextBlock[root->lv],r,0,x)) continue;
			//child node 만들기
			RecNode *child;
			child = malloc(sizeof(RecNode));
			child->lv = root->lv + 1;
			child->rec_score = root->rec_score;
			root->child[r * 15 + x+2] = child;
			for (int i = 0; i < HEIGHT; i++)
				for (int j = 0; j < WIDTH; j++)
					child->f[i][j] = root->f[i][j];
			child->parent = root;

			// 블럭의 y좌표 찾기

			int y=0;
		
			for(y=0;y<HEIGHT;y++){
				int i,j;
				for(i=0;i<4;i++){
					for(j=0;j<4;j++){
						if(block[nextBlock[root->lv]][r][i][j]&&(y+i>=0)&&child->f[y+i][x+j])break;
						if(block[nextBlock[root->lv]][r][i][j]&&(y+i>=HEIGHT)) break;
					}
					if(j<4) break;
				}
				if(i<4)break;
			}
			y--;

			child->x = x;
			child->y = y;
			child->r = r;

			// field에 추가
			int touched = 0;
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (block[nextBlock[root->lv]][r][i][j]){
						if (0 <= y + i && y + i < HEIGHT && 0 <= x + j && x + j < WIDTH){
							if (y + i == HEIGHT - 1||child->f[y+i+1][x+j])
								touched++;
						}
					}
				}
			}
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (block[nextBlock[root->lv]][r][i][j]){
						if (0 <= y + i && y + i < HEIGHT && 0 <= x + j && x + j < WIDTH){
							child->f[y + i][x + j] = 1;
						}
					}
				}
			}
			child->rec_score += touched * 10;

			// line을 지워줌
			int count_line = 0;
			for (int i = 0; i < HEIGHT; i++){
				int delete_flag = 1;
				for (int j = 0; j < WIDTH; j++)
					if (child->f[i][j] == 0){
						delete_flag = 0;
						break;
					}
				if (delete_flag){
					count_line++;
					for (int j = i - 1; j >= 0; j--)
						for (int k = 0; k < WIDTH; k++)
							child->f[j + 1][k] = child->f[j][k];
				}
			}
			child->rec_score += count_line * count_line * 100;
			// 가장 큰 점수로 recomanded 변경
			if (REC_MAX_SCORE < child->rec_score){
				REC_MAX_SCORE = child->rec_score;
				RecNode *curr = child;
				while(1){
					if(curr->lv<=1)
						break;
					curr=curr->parent;
				}
				recommendX = curr->x;
				recommendY = curr->y;
				recommendR = curr->r;
			}

			//recursive call
			recommend(child);
		}
	}
}

//3주차 숙제
void modified_recommend(RecNode *root){
	Rec_size+=sizeof(field)+sizeof(int)*5+sizeof(RecNode*)*max_state[nextBlock[root->lv]]+sizeof(RecNode*);
	for (int i = 0; i < max_state[nextBlock[root->lv]]; i++){
		int x = block_to_field[nextBlock[root->lv]][i].x;
		int r = block_to_field[nextBlock[root->lv]][i].r;
		if (!CheckToMove(root->f, nextBlock[root->lv], r, 0, x))
			continue;
		// child node 만들기
		RecNode *child;
		child = malloc(sizeof(RecNode));
		child->lv = root->lv + 1;
		child->rec_score = root->rec_score;
		root->child[i] = child;
		memcpy(child->f, root->f, sizeof(field));
		child->parent = root;
		
		// 블럭의 y좌표 찾기

		int y = 0;
		for (; y < HEIGHT; y++){
			int i, j;
			for (i = 0; i < 4; i++){
				for (j = 0; j < 4; j++){
					if (block[nextBlock[root->lv]][r][i][j] && (y + i >= 0) && child->f[y + i][x + j])
						break;
					if (block[nextBlock[root->lv]][r][i][j] && (y + i >= HEIGHT))
						break;
				}
				if (j < 4)
					break;
			}
			if (i < 4)
				break;
		}
		y--;

		child->x = x;
		child->y = y;
		child->r = r;

		// field에 추가
		int touched = 0;
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				if (block[nextBlock[root->lv]][r][i][j]){
					if (0 <= y + i && y + i < HEIGHT && 0 <= x + j && x + j < WIDTH){
						if (y + i == HEIGHT - 1 || child->f[y + i + 1][x + j])
							touched++;
					}
				}
			}
		}
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				if (block[nextBlock[root->lv]][r][i][j]){
					if (0 <= y + i && y + i < HEIGHT && 0 <= x + j && x + j < WIDTH){
						child->f[y + i][x + j] = 1;
					}
				}
			}
		}
		child->rec_score += touched * 10; // 이 값을 변경가능


		child->rec_score -= hole(child->f);

		// line을 지워줌
		int count_line = 0;
		for (int i = 0; i < HEIGHT; i++){
			int delete_flag = 1;
			for (int j = 0; j < WIDTH; j++)
				if (child->f[i][j] == 0){
					delete_flag = 0;
					break;
				}
			if (delete_flag){
				count_line++;
				for (int j = i - 1; j >= 0; j--)
					for (int k = 0; k < WIDTH; k++)
						child->f[j + 1][k] = child->f[j][k];
			}
		}
		child->rec_score += count_line * count_line * 100;
		
		// 가장 큰 점수로 recomanded 변경
		if (child->lv == DEPTH)
			if (REC_MAX_SCORE <= child->rec_score){
				REC_MAX_SCORE = child->rec_score;
				RecNode *curr = child;
				while (1){
					if (curr->lv <= 1)
						break;
					curr = curr->parent;
				}
				//if (REC_MAX_Y < curr->y){
					recommendX = curr->x;
					recommendY = curr->y;
					recommendR = curr->r;
					REC_MAX_Y = curr->y;
				//}
			}

		// recursive call
		if (child->lv < DEPTH)
			modified_recommend(child);
	}
}

void recommendedPlay(){
	// user code
	rec_flag=1;
	init=time(NULL);
	play();
	double rec_time=(double)difftime(end,init);
	double rec_memory=(double)Rec_size/1024;
	clear();
	move(0,0);
	printw("time           : %10.2lf sec\n",rec_time);
	printw("score          : %10d Point\n",score);
	printw("memory         : %10.2lf KB\n",rec_memory);
	printw("score / sec    : %10.2lf P/sec\n",(double)score/rec_time);
	printw("score / memory : %10.2lf P/KB\n",(double)score/rec_memory);
	getch();
	rec_flag=0;
}

int hole(char f[HEIGHT][WIDTH]){
	int hole_count=0,hole_flag=0;
	int re=0;
	int weight=200;

	for(int x=0;x<WIDTH;x++){
		hole_count=0;
		hole_flag=0;
		for(int y=0;y<HEIGHT;y++){
			if(f[y][x]){
				hole_flag=1;
				re+=hole_count*weight;
				hole_count=0;
			}
			else{
				hole_count+=hole_flag;
			}
		}
	}


	return re;
}