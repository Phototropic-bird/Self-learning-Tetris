//-------------------------------------
//控制方塊在所傳入的陣列座標中移動的標頭檔之函式部分
//-------------------------------------

#include <bits/stdc++.h>
#include "MyObject.h"

bool if_fit(std::pair<float, float>, int, int, int*, int); //brick center, shape of the piece, cur_spin
void wall_kick(std::pair<float, float>&, int, int&, int&, int, int*, int);    //center, shape, cur_spin, pre_spin, if_spin, vis, rows

//紀錄的不同形狀對應到不同旋轉方向的座標
std::pair<float, float> states[7][4][4] = {
    {  //S shape
      {{0.f , 0.f}, {-1.f, 0.f }, {0.f , -1.f}, {1.f , -1.f}},      //S- 0 spin
      {{0.f , 0.f}, {0.f , -1.f}, {1.f , 0.f }, {1.f , 1.f }},      //S- 1 spin
      {{0.f , 0.f}, {1.f , 0.f }, {0.f , 1.f }, {-1.f, 1.f }},      //S- 2 spin
      {{0.f , 0.f}, {0.f , 1.f }, {-1.f, 0.f }, {-1.f, -1.f}}       //S- 3 spin
    },
    {  //Z shape
      {{0.f , 0.f}, {1.f  , 0.f }, {0.f , -1.f}, {-1.f , -1.f}},      //Z- 0 spin
      {{0.f , 0.f}, {0.f  , 1.f }, {1.f , 0.f }, {1.f  , -1.f}},      //Z- 1 spin
      {{0.f , 0.f}, {-1.f , 0.f }, {0.f , 1.f }, {1.f  , 1.f }},      //Z- 2 spin
      {{0.f , 0.f}, {0.f  , -1.f}, {-1.f, 0.f }, {-1.f , 1.f }}       //Z- 3 spin
    },
    { //T shape
      {{0.f, 0.f}, {0.f, -1.f}, {1.f, 0.f}, {-1.f, 0.f}},             //T- 0 spin
      {{0.f, 0.f}, {0.f, -1.f}, {0.f, 1.f}, { 1.f, 0.f}},             //T- 1 spin
      {{0.f, 0.f}, {0.f,  1.f}, {1.f, 0.f}, {-1.f, 0.f}},             //T- 2 spin
      {{0.f, 0.f}, {0.f, -1.f}, {0.f, 1.f}, {-1.f, 0.f}}              //T- 3 spin
    },
    { //O shape
      {{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}},   //O- are the same
      {{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}},
      {{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}},
      {{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}},
    },
    { //L shape
      {{0.f, 0.f}, {1.f, 0.f}, {-1.f, 0.f}, {1.f, -1.f}},             //L- 0 spin
      {{0.f, 0.f}, {0.f,-1.f}, {0.f , 1.f}, {1.f,  1.f}},             //L- 1 spin
      {{0.f, 0.f}, {-1.f, 0.f}, {-1.f, 1.f}, {1.f, 0.f}},             //L- 2 spin
      {{0.f, 0.f}, {0.f, -1.f}, {-1.f, -1.f}, {0.f, 1.f}}             //L- 3 spin
    },
    { //J shape
      {{0.f, 0.f}, {-1.f, 0.f}, {1.f,  0.f},  {-1.f, -1.f}},          //J- 0 spin
      {{0.f, 0.f}, {0.f, -1.f}, {1.f, -1.f},  {0.f ,  1.f}},          //J- 1 spin
      {{0.f, 0.f}, {1.f,  0.f}, {1.f,  1.f},  {-1.f,  0.f}},          //J- 2 spin
      {{0.f, 0.f}, {0.f, -1.f}, {0.f , 1.f},  {-1.f,  1.f}}           //J- 3 spin
    },
    { //I shape
      {{0.5f, -1.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {0.5f, 1.5f}},     //I- 0 spin
      {{-1.5f, 0.5f}, {-0.5f, 0.5f}, {0.5f, 0.5f}, {1.5f, 0.5f}},     //I- 1 spin
      {{-0.5f,-1.5f}, {-0.5f,-0.5f}, {-0.5f, 0.5f}, {-0.5f, 1.5f}},   //I- 2 spin
      {{-1.5f,-0.5f}, {-0.5f,-0.5f}, {0.5f,-0.5f}, {1.5f,-0.5f}}      //I- 3 spin
    }
};
//當方塊旋轉，與地圖有矛盾時。可以移動中心到一些相鄰地方檢查是否合適
 std::pair<int,int> wall_kicking_table[17][5] = {
    {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
    {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
    {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
    {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
    {{0,0},{1,0},{1,-1},{0,2},{1,2}},
    {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
    {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
    {{0,0},{1,0},{1,-1},{0,2},{1,2}},

    {{0,0},{-1,0},{1,0},{0,-1}},     //wall kicks for 180turn

    {{0,0}, {-2,0}, {1,0}, {-2,1}, {1,-2}}, //wall kicks for I
    {{0,0}, {2,0}, {-1,0}, {2,-1}, {-1,2}},
    {{0,0}, {-1,0}, {2,0}, {-1,-2}, {2,1}},
    {{0,0}, {1,0}, {-2,0}, {1,2}, {-2,-1}},
    {{0,0}, {2,0}, {-1,0}, {2,-1}, {-1,2}},
    {{0,0}, {-2,0}, {1,0}, {-2,1}, {1,-2}},
    {{0,0}, {1,0}, {-2,0}, {1,2}, {-2,-1}},
    {{0,0}, {-1,0}, {2,0}, {-1,-2}, {2,1}}
};
//判斷玩家是否用踢牆做出T轉
std::pair<int, int> T_spinTable[6][3]{
    {{-1, -1},{2, 0},{2, 0}},   //0
    {{1, -1},{-2, 0},{-2, 0}},  //0
    {{1, 1},{-1, 1},{-1, -1}},  //2
    {{-1, 1},{1, 1},{1, -1}},   //2
    {{1, -1},{-1, 0},{-1, 0}},  //R (1)
    {{-1, -1},{1, 0},{1, 0}},   //L (3)
};


void MyObject::initiating(int shape, int rows, int colums, int* vis){
    this->cur_spin = 0;
    this->pre_spin = 0;
    this->shape = shape;
    this->rows = rows;
    this->colums = colums;
    this->vis = vis;
}

MyObject::MyObject(int shape, int kind, int rows, int colums, int* vis){
    this->rows = rows;
    this->setShape(shape);
    this->kind = kind;
    this->colums = colums;
    this->vis = vis;
};
MyObject::~MyObject(){};
int MyObject::move(int dx, int dy){
    bool able_to_move=1;
    int down_contradation = 0;
    for(int i=0; i<4; i++){
        bool can_I_move = if_fit({this->center.first + dx, this->center.second + dy}, this->shape, this->cur_spin, this->vis, this->rows);
        if( ! can_I_move ){
            able_to_move=0;
            break;
        }
    }
    if(able_to_move){
        this->center.first+=dx;
        this->center.second+=dy;
    }
    else  if( dy == 1 ) down_contradation = 1;    //down contradation times

    return down_contradation;
}
bool MyObject::spin(int d){   //return true when its t-spin
    bool rt = 0;
    int if_spin = this->cur_spin+d;
    if(if_spin>=4) if_spin%=4;
    else if(if_spin<0) if_spin=3;

    if(if_fit(this->center,this->shape,if_spin, this->vis, this->rows)){
        this->pre_spin=this->cur_spin;
        this->cur_spin=if_spin;
    }else wall_kick(this->center, this->shape, this->cur_spin, this->pre_spin, if_spin, this->vis, this->rows);
    
    if(this->shape == 2){   //t-spin judgement
        int its_centr_x = this->center.first;
        int its_centr_y = this->center.second;
        int w,s;
        if(this->cur_spin == 0){
            w=0;
            s=2;
        }else if(this->cur_spin == 1){
            w=4;
            s=5;
        }else if(this->cur_spin == 2){
            w=2;
            s=4;
        }else{
            w=5;
            s=6;
        }
        for(int i=w; i<s; i++){
            rt = 1;
            for(int j=0; j<3; j++){
                if(this->vis[(its_centr_y+T_spinTable[i][j].second)*(this->rows+2)+its_centr_x+T_spinTable[i][j].first] == -1) rt = 0;
            }
            if(rt){
                break;
            }
        }
    }
    return rt;
}
std::vector<std::pair<float,float>> MyObject::getBrick(){
    std::vector<std::pair<float,float>> vc;
    for(int i=0; i<4; i++){
        vc.push_back({this->center.first+states[this->shape][this->cur_spin][i].first, this->center.second+states[this->shape][this->cur_spin][i].second});
    }
    return vc;
}
void MyObject::caculateDownShadow(){
    int min_moves_to_touch=INT_MAX;
    for(int i=0; i<4; i++){
        int its_x=this->center.first+states[this->shape][this->cur_spin][i].first;
        int its_y=this->center.second+states[this->shape][this->cur_spin][i].second;
        int moves=0;
        while(vis[(its_y)*(rows+2)+its_x]==-1){
            its_y++;
            moves++;
           }
        moves--;
        min_moves_to_touch = std::min(min_moves_to_touch, moves);
    }
    this->shadowCenter.first = this->center.first;
    this->shadowCenter.second = this->center.second + min_moves_to_touch;
}
std::vector<std::pair<float,float>> MyObject::getShadow(){
    std::vector<std::pair<float,float>> vc;
    for(int i=0; i<4; i++){
        vc.push_back({this->shadowCenter.first+states[this->shape][this->cur_spin][i].first, this->shadowCenter.second+states[this->shape][this->cur_spin][i].second});
    }
    return vc;
}
void MyObject::setShape(int shape){
    if(shape>= 0 && shape<=6){
        this->shape = shape;
        this->cur_spin = 0;
        this->pre_spin = 0;
        if(this->shape==3)                  center = {(this->rows)/2+0.5f, 1.5f};   //O 
        else if(this->shape==6)             center = {(this->rows)/2+0.5f, 2.5f};   //I
        else                          center = {(this->rows)/2 , 2.f}; 
    }
    else std::cout<<shape<<" Wrong in setting shape";
}
int MyObject::getShape(){
    return this->shape;
}
void MyObject::setKind(int kind){
    this->kind = kind;
}
int MyObject::getKind(){
    return this->kind;
}

void MyObject::setShadowCenter(std::pair<float, float> ct){
    this->shadowCenter = ct;
}

bool if_fit(std::pair<float, float> brick_center, int s, int the_spin, int* vis, int rows){ //check if the operation can be performed   
    bool fit=1;
    for(int i=0; i<4; i++){
        int after_x=brick_center.first+states[s][the_spin][i].first;
        int after_y=brick_center.second+states[s][the_spin][i].second;
        if(vis[(after_y)*(rows+2)+after_x]!=-1){
            fit=0;
            break;
        }
    }
    if(fit){    //able to spin
        return 1;
    }else return 0;
}
void wall_kick(std::pair<float, float>& brick_center, int shape, int& cur_spin, int& pre_spin, int if_spin, int* vis, int rows){
    bool able_to_spin=0;
    int i;
    if(abs(if_spin-cur_spin)==2){   //turn 180 //return right away
        for(i=0; i<4; i++){
            able_to_spin=if_fit({brick_center.first+wall_kicking_table[8][i].first,brick_center.second+wall_kicking_table[8][i].second},shape,if_spin, vis, rows);
            if(able_to_spin){
                brick_center.first+=wall_kicking_table[8][i].first;
                brick_center.second+=wall_kicking_table[8][i].second;
                pre_spin=cur_spin;
                cur_spin=if_spin;
                break;
            }
        }
        return;
    }else if(shape != 6){ //not I //位移後放入if_fit，看看可不可行
        if(cur_spin==0 && if_spin==1){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[0][i].first,brick_center.second+wall_kicking_table[0][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[0][i].first;
                    brick_center.second+=wall_kicking_table[0][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==1 && if_spin==0){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[1][i].first,brick_center.second+wall_kicking_table[1][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[1][i].first;
                    brick_center.second+=wall_kicking_table[1][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==1 && if_spin==2){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[2][i].first,brick_center.second+wall_kicking_table[2][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[2][i].first;
                    brick_center.second+=wall_kicking_table[2][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==2 && if_spin==1){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[3][i].first,brick_center.second+wall_kicking_table[3][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[3][i].first;
                    brick_center.second+=wall_kicking_table[3][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==2 && if_spin==3){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[4][i].first,brick_center.second+wall_kicking_table[4][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[4][i].first;
                    brick_center.second+=wall_kicking_table[4][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==3 && if_spin==2){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[5][i].first,brick_center.second+wall_kicking_table[5][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[5][i].first;
                    brick_center.second+=wall_kicking_table[5][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==3 && if_spin==0){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[6][i].first,brick_center.second+wall_kicking_table[6][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[6][i].first;
                    brick_center.second+=wall_kicking_table[6][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==0 && if_spin==3){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[7][i].first,brick_center.second+wall_kicking_table[7][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[7][i].first;
                    brick_center.second+=wall_kicking_table[7][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }
    }else{  //I
        if(cur_spin==0 && if_spin==1){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[9][i].first,brick_center.second+wall_kicking_table[9][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[9][i].first;
                    brick_center.second+=wall_kicking_table[9][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==1 && if_spin==0){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[10][i].first,brick_center.second+wall_kicking_table[10][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[10][i].first;
                    brick_center.second+=wall_kicking_table[10][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==1 && if_spin==2){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[11][i].first,brick_center.second+wall_kicking_table[11][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[11][i].first;
                    brick_center.second+=wall_kicking_table[11][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==2 && if_spin==1){
            for(int i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[12][i].first,brick_center.second+wall_kicking_table[12][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[12][i].first;
                    brick_center.second+=wall_kicking_table[12][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==2 && if_spin==3){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[13][i].first,brick_center.second+wall_kicking_table[13][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[13][i].first;
                    brick_center.second+=wall_kicking_table[13][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==3 && if_spin==2){
            for(int i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[14][i].first,brick_center.second+wall_kicking_table[14][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[14][i].first;
                    brick_center.second+=wall_kicking_table[14][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==3 && if_spin==0){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[15][i].first,brick_center.second+wall_kicking_table[15][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[15][i].first;
                    brick_center.second+=wall_kicking_table[15][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }else if(cur_spin==0 && if_spin==3){
            for(i=0; i<5; i++){
                able_to_spin=if_fit({brick_center.first+wall_kicking_table[16][i].first,brick_center.second+wall_kicking_table[16][i].second},shape,if_spin, vis, rows);
                if(able_to_spin){
                    brick_center.first+=wall_kicking_table[16][i].first;
                    brick_center.second+=wall_kicking_table[16][i].second;
                    pre_spin=cur_spin;
                    cur_spin=if_spin;
                    break;
                }
            }
        }
    }
    
}
