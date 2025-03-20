//-------------------------------------
//Tetris 主函式 (main.cpp)
//-------------------------------------

//0-based

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <bits/stdc++.h>
#include <time.h>
#include <random>
#include <windows.h>
#include <chrono>
#include <algorithm>
#include "MyObject.h"
#include "MyObject.cpp"
#define Max_countradtion_times 3
#define genrAmo 7 //現有的方塊類型數
#define nextAmo 5
#define moving_speed 0.1f
#define feedbackFadSpeed 0.1
#define line_cleaning_speed 100 //ms
#define SandFallingSpeed 50    //ms
#define SandFallingFrame 0.2    //size_of_brick
#define BosonFallingSpeed 50    //ms
#define BosonFallingFrame 0.25 //size of brick
#define BosonBrakingingFrame 0.2    //size_of_brick
#define BHSuckingFrame 0.25 //size of brick
#define BHSuckingSpeed 200    //ms
#define MowermovingFrame 0.25 //size of brick
#define MowermovingSpeed 50 //ms
#define MowerBrakingingFrame 0.2    //size of brick
#define NukeBeeingSpeed 100 //ms
#define NukeWhiteSpeed 100 //ms

void ChooseGamemode();
void IniVis();
void IniNxandBagandGenre();
void IniBank();

void IniALL();

int grNum(int, int);
void pushPointerandShuffle();
void anchoring(MyObject*);

void DrawLosearea();
void DrawLattice();
void DrawDroppedOj();   //MyObject that has been set in vis
void DrawMovingOj();   //curObj, Obj and shadow
void DrawNextOj();    //next Obj, nxOb.front()
void DrawHoldOj();   //draw held MyObject
void PrintText();    //hold, lines, score
void PrintFeedback();
void Drawothers();  //draw all the stuff but moving shape and its shadow

void PrintLoseState();

void CheckIfClean();    //推入lineToClean
void LineClean();   //paralyze temporarily
void fixVisBcBk();  //fix vis, block count, bank
void GiveFeedback(bool); //以linetoclean的size 和 t_spin 賦予 score, feedback 值    //bool是用來判斷if_change是否被啟動

void SigninOj(int, std::pair<int,int>);
void SignoutOj(int, std::pair<int,int>);

bool SandEffect();  //index : 1
bool BlackholeEffect();//index : 2 
void BosonEffect(); //index : 3
void MowerEffect(); //index : 4
void ClassmateEffect(); //index : 5
bool NukeEffect(); //index : 6

void defTextures(); //define textures according to the gamemode


//Sounds

//0 for normal //1 for sand //2 for black hole  //3 for boson   // 4 for mower     //5 for classmate    // 6 for nuke
sf::SoundBuffer sounds[genrAmo]{
    sf::SoundBuffer("drop_normal.mp3"),
    sf::SoundBuffer("drop_sand.mp3"),
    sf::SoundBuffer("drop_BH.mp3"),
    sf::SoundBuffer("drop_boson.mp3"),
    sf::SoundBuffer("drop_mower.mp3"),
    sf::SoundBuffer("drop_classmate.mp3"),
    sf::SoundBuffer("drop_nuke.mp3")
};
sf::SoundBuffer line_clean_sound("line_clean.mp3");
sf::SoundBuffer BH_suck_sound("BH_sucking.mp3");
sf::SoundBuffer boom_sound("nuke_boom.mp3");
sf::SoundBuffer nk_ini_sound("nuke_ini_sound.mp3");
sf::Sound sd(sounds[0]);


//keys
bool R_key = 0 ;
bool P_key = 0 ;
bool A_key = 0 ;
bool D_key = 0 ;
bool W_key = 0 ;
bool S_key = 0 ;
bool Space_key = 0 ;
bool Left_key = 0 ;
bool Right_key = 0 ;
bool Up_key = 0;

//rows and colums
int rows = 10;
int colums = 20;
int vis[2500]; 

//gamemode
int Gamemode = -1;
bool isOver = 0;
bool onMousePress = 0;
bool onMouseRelease = 0;
bool isMousePressed = 0;
bool isPressedinside = 0;
bool isPressedinside2 = 0;


//bags
int Bp = 0;
int Gp = 0;
std::vector<int> bag = {0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6};
std::vector<int> genre;

std::deque<std::pair<int,int>> nxOb;    //shape, kind

//about game
float size_of_brick;
int droppingSpeed = 0.5;
bool lose = 0;
bool pause = 0;
bool paralyze = 0;
int dropped = 1;    //0 NOT yet dropped, 1 normal dropped, 2 hard dropped
int cttimes = 0;
int lines = 0;
int score = 0;
std::pair<int,int> holdInfo = {-1,-1};
MyObject *curObj;

//BANKS
//anchoring effect don't need to be in banks
//lasting effect will always be in banks until it was destroyed
std::vector<std::pair<int, int>> BHbank;    //blackhole bank
std::vector<std::pair<int, int>> Sdbank;    //sand bank
//cleaning effect will be in banks only when it was activated
std::vector<std::pair<int, int>> AcNkbank; //activated nukes bank;

//Text
int feedback = -1; 
bool t_spin = 0;
//-1 = non, 0 = single, 1 = double, 2 = triple, 3 = tetris, 4 = t-spin mini, 5 = t-spin single, 6 = t-spin double, 7 = t-spin triple
int tetrisB2B = 0;
int combo = 0;
float alpha = 255; //upon clean new lines, reset alpha to 255

//fonts
const sf::Font fonts[1] =
{
    sf::Font("BAUHS93.ttf")
};

//textures
std::vector<sf::Texture> textures;
sf::Texture recIni("tiles_S.jpg");

//about drawings
    //window
    int window_w, window_h;
    float origin_x, origin_y;
        //lattices
    sf::Color lattice_color = sf::Color(255,255,255, 120);
    float line_thickness = 3.f;    //in pixels
        //MyObject
    sf::Sprite rec(recIni);    // rec size = the initial given texture Intrec
    sf::Vector2f school_scale;
    sf::RectangleShape Shadow_rec;
    sf::Color shadowColor = sf::Color(0, 200, 0, 70);


//line cleaning
int blockCount[100] = {0};
std::vector<int> lineToClean;

//timer's
auto sf_st = std::chrono::system_clock::now();
auto sf_ed = std::chrono::system_clock::now();
auto left_st = std::chrono::system_clock::now();
auto left_ed = std::chrono::system_clock::now();
auto right_st = std::chrono::system_clock::now();
auto right_ed = std::chrono::system_clock::now();
auto softDrop_st = std::chrono::system_clock::now();
auto softDrop_ed = std::chrono::system_clock::now();

//rand seed
std::random_device rd;
std::mt19937 g(rd());

//the window
sf::RenderWindow window(sf::VideoMode({600, 800}), "Tetris ! ");

int main(){
    Shadow_rec.setFillColor(shadowColor);
    srand(time(NULL));

    while(window.isOpen()){
        window_h = window.getSize().y;
        window_w = window.getSize().x;
        size_of_brick = std::min(window_h, window_w) / 40 ;
        origin_x = window_w/2 - size_of_brick*(rows/2+1);
        origin_y = window_h/2 - size_of_brick*(rows/2+5);

        school_scale = {size_of_brick/480.f, size_of_brick/472.f} ;
        Shadow_rec.setSize({size_of_brick, size_of_brick});
        rec.setScale(school_scale);


/*************************    Choosing gamemode     ************/
        onMousePress = 0;
        onMouseRelease = 0;
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            if(not isMousePressed)
                onMousePress = 1;
            isMousePressed = 1;
        }else{
            if(isMousePressed)
                onMouseRelease = 1;
            isMousePressed = 0;
        }
        //人不太可能在從上方if句到下方choosegamemode之間的時間內進入方塊，所以在外面壓移進方塊不太會產生錯誤判斷
        if(Gamemode == -1){
            ChooseGamemode();
            defTextures(); //define textures according to the gamemode
            IniALL();
        }

        if(not isMousePressed){
            isPressedinside = 0;
            isPressedinside2 = 0;
        }

        //events
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
            else if(event->is<sf::Event::Resized>()){
                sf::View view(sf::FloatRect({0.f,0.f},sf::Vector2f(window.getSize())));
                window.setView(view);
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)){  //restart the game
            if(! R_key){
                R_key = 1;
                // restart the game
                Gamemode = -1;
            }
        }else R_key = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::P)){  //pause the game
            if(! P_key){
                P_key = 1;
                // resume the game
                pause = ! pause;
            }
        }else P_key = 0;

        if(lose || pause || Gamemode == -1) paralyze = 1;
        else paralyze = 0;

        if(! paralyze){
/******************************    GENERATE     ********************/
            if( dropped ){
                cttimes = 0;
                dropped = 0;
                delete(curObj);
                //generate a new MyObject
                curObj = new MyObject(nxOb.front().first, nxOb.front().second, rows, colums, vis);
                nxOb.pop_front();
                if(nxOb.size() <= nextAmo){   //沒有高幼祥，可以推
                    nxOb.push_back({bag[Bp], genre[Gp]});
                }
                //push pointer and shuffle
                pushPointerandShuffle();
            }

/****************************    OPERATION     ********************/

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)){  //harddrop
                if(!W_key){
                    dropped = 2;    //harddrop
                    W_key = 1;
                }
            }else W_key = 0 ;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)){  //move left
                if(!A_key){
                    curObj->move(-1, 0);
                    left_st = std::chrono::system_clock::now();
                    A_key = 1;
                }else{
                    left_ed = std::chrono::system_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(left_ed-left_st);
                    if((float(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den) > moving_speed){
                        left_st = std::chrono::system_clock::now();
                        curObj->move(-1, 0);
                    }
                }
            }else A_key = 0 ;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)){  //move right
                if(!D_key){
                    curObj->move(1, 0);
                    right_st = std::chrono::system_clock::now();
                    D_key = 1;
                }else{
                    right_ed = std::chrono::system_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(right_ed-right_st);
                    if((float(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den) > moving_speed){
                        right_st = std::chrono::system_clock::now();
                        curObj->move(1, 0);
                    }
                }
            }else D_key = 0 ;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)){  //softDrop
                if(!S_key){
                    cttimes += curObj->move(0, 1);
                    softDrop_st = std::chrono::system_clock::now();
                    S_key = 1;
                }else{
                    softDrop_ed = std::chrono::system_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(softDrop_ed-softDrop_st);
                    if((float(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den) > moving_speed){
                        softDrop_st = std::chrono::system_clock::now();
                        cttimes += curObj->move(0, 1);
                    }
                }
            }else S_key = 0 ;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)    ){   //clockwise spin
                if(!Right_key){
                    t_spin = curObj->spin(1);
                    Right_key=1;
                }  
            }else Right_key=0;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) ){  //counterclockwise spin
                if(!Left_key){
                    t_spin = curObj->spin(-1);
                    Left_key=1;
                }
            }else Left_key=0;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)   ){    //spin 180
                if(!Up_key){
                    curObj->spin(2);
                    Up_key=1;
                }
            }else Up_key=0;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)){   //hold
                if(!Space_key){
                    if(holdInfo.first == -1){   //put into hold
                        holdInfo = {curObj->getShape(), curObj->getKind()};
                        curObj->setShape(nxOb.front().first);
                        curObj->setKind(nxOb.front().second);
                        nxOb.pop_front();
                        nxOb.push_back({bag[Bp], genre[Gp]});
                        pushPointerandShuffle();
                    }else{  //swap with hold
                        std::pair<int,int> tmp = holdInfo;
                        holdInfo = {curObj->getShape(), curObj->getKind()};
                        curObj->setShape(tmp.first);
                        curObj->setKind(tmp.second);
                    }
                    Space_key=1;
                }
            }else Space_key=0;

            //drop automatically  
            sf_ed = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(sf_ed-sf_st);
            if((float(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den) >= 0.5){
                sf_st=std::chrono::system_clock::now();
                if(!dropped) cttimes+=curObj->move(0, 1);
            }
            if(cttimes == Max_countradtion_times) dropped = 1;

/****************************    Anchoring and Effects    ********************/
            bool if_change = 0;
            if(dropped){     
                score += 10;
                /******  Anchoring EFFECTS  *****/  //effects that happen upon anchoring
                if(curObj->getKind()==5 && Gamemode==0) ClassmateEffect(); //做高宥翔
                else if(curObj->getKind()==3 && Gamemode==0) BosonEffect(); //do boson
                else if(curObj->getKind()==4 && Gamemode==0) MowerEffect(); //do mower
                anchoring(curObj);
                bool rpt = 0; //阻止if_change斷combo
                do{
                    if_change = 0;
                    /******  Lasting EFFECTS  *****/
                    if(Gamemode == 0) if_change = SandEffect();   //paralyze temporarily
                    if(Gamemode == 0) if_change = BlackholeEffect();  //paralyze temporarily
                    /******  Clean Lines  *****/
                    CheckIfClean();
                    GiveFeedback(rpt);
                    if( ! lineToClean.empty()){ //there's line to be cleaned
                        alpha = 255;
                        if_change = 1;
                        LineClean();    //paralyze temporarily
                        if(Gamemode == 0) NukeEffect();   //paralyze temporarily
                        fixVisBcBk();
                        lineToClean.clear();
                    }else if(t_spin){   //消0行但有T_spin
                        alpha = 255;
                        feedback = 4;   //t_spin mini
                    }
                    rpt = 1;
                }while(if_change);
                t_spin = 0;
/****************************    Lose Judement     ********************/
                for(int i=1; i<=4; i++){
                    for(int j=4; j<=7; j++){
                        if(vis[i*(rows+2)+j] != -1){
                            lose = 1;
                            break;
                        }
                    }
                }
            }
/****************************    Drawings and Texts     ********************/
            window.clear(sf::Color(169,169,169));
            if(alpha>0) alpha -= 0.05;
            Drawothers();
            DrawMovingOj();
            window.display();
        }
        if(lose) PrintLoseState();
    }
}
void IniALL(){  //不包括 gamemode = -1
    memset(blockCount, 0, sizeof(blockCount));
    IniVis();
    IniNxandBagandGenre();
    IniBank();
    lines = 0;
    score = 0;
    t_spin = 0;
    feedback = -1;
    tetrisB2B = 0;
    combo = 0;
    alpha = 255;
    holdInfo = {-1,-1};
    dropped = 1;
    pause = 0;
    lose = 0;
}

void ChooseGamemode(){
    window.clear(sf::Color(169,169,169));
    sf::RectangleShape st_rec({(float)window_w/4, (float)window_h/6});
    sf::Text tx(fonts[0]);
    tx.setCharacterSize(std::min(window_w, window_h)/20);
    tx.setString(" Special ");
    tx.setFillColor(sf::Color::Black);
    tx.setOrigin({(float)tx.getCharacterSize(), tx.getCharacterSize()/2.f});
    tx.setPosition({(float)window_w/2, window_h/2-size_of_brick*8});
    st_rec.setFillColor(sf::Color::White);
    st_rec.setOrigin(st_rec.getGeometricCenter());
    st_rec.setPosition({(float)window_w/2, window_h/2-size_of_brick*8});
    auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));
    if(st_rec.getGlobalBounds().contains(mouse_position)){
        st_rec.setScale({1.5f,1.5f});
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) and isPressedinside)
            st_rec.setFillColor(sf::Color(255,255,0));  //進入且按
        else st_rec.setFillColor(sf::Color(0,0,255));   //進入
        if(onMousePress){
            isPressedinside = 1;
        }
        if(onMouseRelease and isPressedinside){
            Gamemode = 0;
        }
        isOver = 1;
    }else{
        isOver = 0;
    }
    window.draw(st_rec);
    window.draw(tx);

    tx.setString(" Normal ");
    tx.setPosition({window_w/2.f, window_h/2-size_of_brick*8 + window_h/4.f});
    st_rec.setPosition({window_w/2.f, window_h/2-size_of_brick*8 + window_h/4.f});
    st_rec.setScale({1.f,1.f});
    st_rec.setFillColor(sf::Color::White);
    if(st_rec.getGlobalBounds().contains(mouse_position)){
        st_rec.setScale({1.5f,1.5f});
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) and isPressedinside2)
            st_rec.setFillColor(sf::Color(255,255,0));  //進入且按
        else st_rec.setFillColor(sf::Color(0,0,255));   //進入
        if(onMousePress){
            isPressedinside2 = 1;
        }
        if(onMouseRelease and isPressedinside2){
            Gamemode = 1;
        }
        isOver = 1;
    }else{
        isOver = 0;
    }
    window.draw(st_rec);
    window.draw(tx);
    window.display();
}

void IniBank(){
    Sdbank.clear();
    BHbank.clear();
    AcNkbank.clear();
}
void IniNxandBagandGenre(){
    nxOb.clear();
    Bp = 0;
    Gp = 0;
    //initiate the genre
    genre.clear();
    if(Gamemode == 0)
    genre = {0,1,5,0,1,5,0,1,5,0,1,5,0,1};
    else
    for(int i=0; i<2; i++) for(int j=genrAmo-1; j>=0; j--) genre.push_back(0);

    //initiate the bag, genre
    std::shuffle(bag.begin(), bag.begin()+7, g);
    std::shuffle(genre.begin(), genre.begin()+genrAmo, g);
    for(int i=0; i<nextAmo; i++){
        nxOb.push_front({bag[i],genre[i]});
        Bp ++;
        Gp ++;
    }
}
void IniVis(){
    //initiate the vis
    for(int i=0; i<2500; i++) vis[i] = -1;
    for(int i=0; i<rows+2; i++){
        vis[i] = -2; //上頂牆
        vis[(colums+5)*(rows+2)+i] = -2; //底邊牆
    }
    for(int i=0; i<colums+6; i++){
        vis[i*(rows+2)] = -2; //左邊牆
        vis[i*(rows+2)+rows+1] = -2; //右邊牆
    }
}
void pushPointerandShuffle(){
    //advance pointer
    Bp++; Bp%=14;
    Gp++; Gp%=genrAmo*2;
    //reshuffle
    if(Bp == 0) std::shuffle(bag.begin()+7, bag.end(), g);
    else if( Bp == 7) std::shuffle(bag.begin(), bag.begin()+7, g);
    if(Gp == 0) std::shuffle(genre.begin()+genrAmo, genre.end(), g);
    else if ( Gp == 7) std::shuffle(genre.begin(), genre.begin()+genrAmo, g);
}
int grNum(int lb, int ub){
    int random = rand();
    int siz = ub-lb+1;
    int fish = random % siz;
    if(fish == 0) return ub;
    else return fish+lb-1;
}

void anchoring(MyObject *oj){   //以Object的shadow center 登入物件

    //anchoring effects happen before anchoring so the sound need to be played sperately
    if(oj->getKind()!=3 & oj->getKind()!=4 & oj->getKind()!=5){ 
        sd.setBuffer(sounds[oj->getKind()]);
        sd.play();
    }
   
    if(curObj->getKind()!=4) oj->caculateDownShadow();  //割草機已經用 MowerEffect 算好影子了
    for(auto p : oj->getShadow()){
        int its_x = p.first;
        int its_y = p.second;
        if(curObj->getKind() == 6){ //核彈在anchoring 時不能進到active bank
            vis[its_y*(rows+2)+its_x] = 6;
            blockCount[its_y] ++ ; 
        }else SigninOj((Gamemode == 0 ? curObj->getKind() : curObj->getShape()), {its_x, its_y}); //cleaning effect don't have to sign in when anchoring
    }
}

void CheckIfClean(){    //推入lineToClean
    for(int i=1; i<colums+5; i++){
        if(blockCount[i] == rows) lineToClean.push_back(i);
    }
}
void LineClean(){   //paralyze temporarily
    sd.setBuffer(line_clean_sound);
    sd.play();
        for(int c = -1; c < rows/2; c++){
            //erase lines in visited
            if(c != -1){    //do not clean in the first round, to show players what they just dropped
                for(auto v : lineToClean){
                    int lf = v*(rows+2)+rows/2-c;
                    int rh = v*(rows+2)+rows/2+1+c;
                    //  DEBANKING or Activate clean effects
                    if(vis[lf] == 6 && Gamemode==0){   //在activate nuke時再 push 就好

                        //nuke ini sound effect
                        sd.setBuffer(nk_ini_sound);
                        sd.play();

                        AcNkbank.push_back({rows/2-c, v});
                    }else SignoutOj(vis[lf], {rows/2-c, v});
                    
                    if(vis[rh] == 6 && Gamemode==0){   //在activate nuke時再 push 就好

                        //nuke ini sound effect
                        sd.setBuffer(nk_ini_sound);
                        sd.play();

                        AcNkbank.push_back({rows/2+1+c, v});
                    }else SignoutOj(vis[rh], {rows/2+1+c, v});
                }
            }
            //reset
            window.clear(sf::Color(169,169,169));
            Drawothers();
            window.display();
            Sleep(line_cleaning_speed);
        }
}
void fixVisBcBk(){
    std::sort(lineToClean.begin(),lineToClean.end());
    for(int v : lineToClean){
        //fix bank
        for(auto it = Sdbank.begin(); it!=Sdbank.end(); it++){
            if((*it).second<v) (*it).second++;
        }
        for(auto it = BHbank.begin(); it!=BHbank.end(); it++){
            if((*it).second<v) (*it).second++;
        }
        

        for(int i=v; i>=2; i--){
            for(int j=1; j<=rows; j++){
                vis[i*(rows+2)+j]=vis[(i-1)*(rows+2)+j];
            }
            blockCount[i] = blockCount[i-1];
        }
        for(int k=1; k<=rows; k++) vis[(rows+2)+k]=-1;
        blockCount[1]=0;
    }
}

void SigninOj(int index, std::pair<int,int> pos){   //in anchoring

    //lasting object and clean object need to be put into set (bank)
    if(index==1 && Gamemode==0) Sdbank.push_back(pos);
    else if(index==2 && Gamemode==0)BHbank.push_back(pos);
    else if(index==6 && Gamemode==0) AcNkbank.push_back(pos);

    vis[pos.second*(rows+2)+pos.first] = index;
    blockCount[pos.second]++;
}
void SignoutOj(int index, std::pair<int,int> pos){ // when deletting MyObjects
    //lasting object and clean object need to be removed out of set (bank)
    if(index==1){
        auto it = std::find(Sdbank.begin(), Sdbank.end(), pos);
        if(it!=Sdbank.end()) Sdbank.erase(it);
    }else if(index==2){
        auto it = std::find(BHbank.begin(), BHbank.end(), pos);
        if(it!=BHbank.end()) BHbank.erase(it);
    }else if(index==6){
        auto it = std::find(AcNkbank.begin(), AcNkbank.end(), pos);
        if(it!=AcNkbank.end()) AcNkbank.erase(it);
    }

    vis[pos.second*(rows+2)+pos.first] = -1;
    blockCount[pos.second]--;
}


bool SandEffect(){  //paralyze temporarily //index : 1
    std::vector<std::pair<float,float>> AniVc;  //animation vector
    std::set<int> ESset;    //empty sand set
    for(auto p : Sdbank) ESset.insert(p.first); 
    for(int v : ESset){
        int flag = 0;
        for(int i=colums+5; i>=1; i--){
            if(vis[i*(rows+2)+v] == -1) flag = 1;   //空格
            else if(vis[i*(rows+2)+v] == 1 && (flag == 1 || flag == 2)){    //空沙塊
                flag = 2;
                AniVc.push_back({v, i});
                SignoutOj(1, {v,i});
            }else flag = 0;
        }
    }
    if(AniVc.empty()) return 0;
    while( ! AniVc.empty() ){   //做動畫，修vis，blockCount
        window.clear(sf::Color(169,169,169));
        Drawothers();
        for(auto it = AniVc.begin(); it!=AniVc.end(); it++){
            (*it).second+=SandFallingFrame;
            rec.setPosition({origin_x + (*it).first*size_of_brick, origin_y + (*it).second*size_of_brick});
            rec.setTexture(textures[1]);
            window.draw(rec);

            //檢查此沙塊是否已到底，如果是的話就將其刪除
            int check_x = (*it).first;
            int check_y = (*it).second; //無條件捨去
            if(vis[(check_y  +  1  )*(rows+2)+check_x] != -1){  //檢查此沙塊下方是否沒物品

                //play drop sand upon dropping sand
                sd.setBuffer(sounds[1]);
                sd.play();

                SigninOj(1, {check_x, check_y});
                AniVc.erase(it);    
                it--;
            }
        }
        window.display();
        Sleep(SandFallingSpeed);
    }
    return 1;
}
bool BlackholeEffect(){ //paralyze temporarily //index : 2 
    bool rt = 0 ;
    std::vector<std::pair<int,int>> vc_up;
    std::vector<int> kd_up;
    std::vector<std::pair<int,int>> vc_down;
    std::vector<int> kd_down;
    //collect the blackholes that need to be operated
    for(auto it =BHbank.begin(); it!=BHbank.end(); it++){
        //clean up
        if(((*it).second)>=2 && vis[((*it).second-1)*(rows+2)+(*it).first]!=-1 
        && vis[((*it).second-1)*(rows+2)+(*it).first]!=2){
            vc_up.push_back({(*it).first, (*it).second-1});
            kd_up.push_back(vis[((*it).second-1)*(rows+2)+(*it).first]);
            SignoutOj(vis[((*it).second-1)*(rows+2)+(*it).first], {(*it).first, (*it).second-1});
        }
        //clean down
        if(vis[((*it).second+1)*(rows+2)+(*it).first]!=-2 && vis[((*it).second+1)*(rows+2)+(*it).first]!=-1
        && vis[((*it).second+1)*(rows+2)+(*it).first]!=2){
            vc_down.push_back({(*it).first, (*it).second+1});
            kd_down.push_back(vis[((*it).second+1)*(rows+2)+(*it).first]);
            SignoutOj(vis[((*it).second+1)*(rows+2)+(*it).first], {(*it).first, (*it).second+1});
        }
    }
    if(vc_up.empty() && vc_down.empty()) return 0;

    //sucking sound effect
    sd.setBuffer(BH_suck_sound);
    sd.play();

    //animation
    for(int i=0; i<4; i++){
        window.clear(sf::Color(169,169,169));
        Drawothers();
        // animate up
        float ss = pow(0.9, i);
        rec.setScale({ss*rec.getScale().x, ss*rec.getScale().y});
        for(int j=0; j<vc_up.size(); j++){
            rec.setTexture(textures[kd_up[j]]);
            rec.setPosition({origin_x + vc_up[j].first* size_of_brick
                , origin_y + (vc_up[j].second)*size_of_brick + (float)(i*BHSuckingFrame*size_of_brick)});
            window.draw(rec);
        }
        // animate down
        for(int j=0; j<vc_down.size(); j++){
            rec.setTexture(textures[kd_down[j]]);
            rec.setPosition({origin_x + vc_down[j].first* size_of_brick
                , origin_y + (vc_down[j].second)*size_of_brick - (float)(i*BHSuckingFrame*size_of_brick)});
            window.draw(rec);
        }
        window.display();
        rec.setScale(school_scale);
        Sleep(BHSuckingSpeed);
    }
    return 1;
}
void BosonEffect(){ //paralyze temporarily //index : 3
    //anchoring sound effect
    sd.setBuffer(sounds[3]);
    sd.play();

    auto vc = curObj->getShadow();
    std::vector<std::pair<int,int>> DesVc;
    std::vector<int> DesKd;
    int min_move = INT_MAX;
    for(auto v : vc){
        int its_x = v.first;
        int its_y = v.second;
        //計算最小需要落下幾格
        min_move = std::min(colums+4 - its_y, min_move);
    }
    //落下動畫
    if(min_move == 0) return;
/*****************這裡boson 都還沒登錄到vis中，main中的anchoring 會做這件事 */
    while(min_move != 0){
        min_move--;
/***********************將boson正下方的方塊清除******************/
        for(auto it = vc.begin(); it!=vc.end(); it++){
            int its_x = (*it).first;
            int its_y = (*it).second;
            //將已經過的從 vis 刪除
            if(vis[(its_y+1)*(rows+2)+its_x] != -1){
                //將要被破壞的方塊蒐集
                DesVc.push_back({its_x, its_y+1});
                DesKd.push_back(vis[(its_y+1)*(rows+2)+its_x]);
                SignoutOj(vis[(its_y+1)*(rows+2)+its_x], {its_x, its_y+1});
            }
        }
/*************************做boson下降效果*********************/
        for(int i=0; i<=4; i++){
            window.clear(sf::Color(169,169,169));
            for(auto it = vc.begin(); it!=vc.end(); it++){
                //畫正在下降的boson
                rec.setTexture(textures[3]);
                rec.setPosition({origin_x + size_of_brick*(*it).first, origin_y + (float)(size_of_brick*(BosonFallingFrame*i+(*it).second))});
                window.draw(rec);
            }
            for(int j=0; j<DesVc.size(); j++){
                rec.setTexture(textures[DesKd[j]]);
                float placement;
                if(DesVc[j].first <= curObj->getShadowCenter().first) placement = -1;
                else placement = 1;
                placement *= BosonBrakingingFrame;
                rec.setPosition({origin_x + (DesVc[j].first + placement*i)*size_of_brick
                    , origin_y + (DesVc[j].second)*size_of_brick + (float)(BosonBrakingingFrame*0.5*i*i*size_of_brick)});
                window.draw(rec);
            }
            Drawothers();
            window.display();
            Sleep(BosonFallingSpeed);
        }
        DesVc.clear();
        DesKd.clear();
/*****************將boson往下一格以利下次判斷****************/
        for(auto it = vc.begin(); it!=vc.end(); it++){
            (*it).second++;
        }
    }
} 
void MowerEffect(){ //paralyze temporarily //index : 4

    //anchoring sound effect
    sd.setBuffer(sounds[4]);
    sd.play();

    auto vc = curObj->getShadow();
    std::vector<std::pair<int,int>> DesVc;
    std::vector<int> DesKd;
    int min_move = INT_MAX;
    int rev;
    for(auto v : vc){
        int its_x = v.first;
        int its_y = v.second;
        //計算最小需要橫移幾格
        min_move = std::min(its_x - 1, min_move);
    }
    //橫移動畫
    if(min_move == 0) return;
    rev = min_move;
/*****************這裡mower 都還沒登錄到vis中，main中的anchoring 會做這件事 */
    while(min_move != 0){
        min_move--;
/***********************將mower正左方的方塊清除******************/
        for(auto it = vc.begin(); it!=vc.end(); it++){
            int its_x = (*it).first;
            int its_y = (*it).second;
            //將已經過的從 vis 刪除
            if(vis[(its_y)*(rows+2)+(its_x-1)] != -1){
                //將要被破壞的方塊蒐集
                DesVc.push_back({its_x-1, its_y});
                DesKd.push_back(vis[(its_y)*(rows+2)+(its_x-1)]);
                SignoutOj(vis[(its_y)*(rows+2)+(its_x-1)], {its_x-1, its_y});
            }
        }
/*************************做mower左移效果*********************/
        for(int i=0; i<=4; i++){
            window.clear(sf::Color(169,169,169));
            for(auto it = vc.begin(); it!=vc.end(); it++){
                //畫正在左移的mower
                rec.setTexture(textures[4]);
                rec.setPosition({origin_x + (float)(size_of_brick*(-MowermovingFrame*i+(*it).first))
                    , origin_y + size_of_brick*(*it).second});
                window.draw(rec);
            }
            for(int j=0; j<DesVc.size(); j++){
                rec.setTexture(textures[DesKd[j]]);
                float placement;
                if(DesVc[j].second <= curObj->getShadowCenter().second) placement = -1;
                else placement = 1;
                placement *= BosonBrakingingFrame;
                rec.setPosition({origin_x + (DesVc[j].first)*size_of_brick - (float)(MowerBrakingingFrame*0.5*i*i*size_of_brick)
                    , origin_y + (DesVc[j].second + placement*i)*size_of_brick});
                window.draw(rec);
            }
            Drawothers();
            window.display();
            Sleep(MowermovingSpeed);
        }
        DesVc.clear();
        DesKd.clear();
/*****************將mower左移一格以利下次判斷****************/
        for(auto it = vc.begin(); it!=vc.end(); it++){
            (*it).first--;
        }
    }

/*******校準 mower 的 shadow center 以利 anchoring 登入*/
    curObj->setShadowCenter({curObj->getShadowCenter().first-rev, curObj->getShadowCenter().second});
} //paralyze temporarily //index : 4
void ClassmateEffect(){ //paralyze temporarily //index : 5
    //anchoring sound effect
    sd.setBuffer(sounds[5]);
    sd.play();

    for(int i=0; i<2; i++)
    nxOb.push_front({curObj->getShape(), grNum(0, genrAmo-1)});
}
bool NukeEffect(){  //paralyze temporarily //index : 6
    if(AcNkbank.empty()) return 0;

    // backup Activated bank
    auto backup = AcNkbank;

    // clean Activate bank
    for(auto p : backup){
        SignoutOj(6, p);
    }
    //閃爍 animation    //sign out AcNkbank 中的座標
    for(int i=0; i<4; i++){
        window.clear(sf::Color(169,169,169));
        for(auto p : backup){
            rec.setTexture(textures[ i%2==0 ? 7 : 6]);  //activated sign when i is even
            rec.setPosition({origin_x + p.first*size_of_brick, origin_y + p.second*size_of_brick});
            window.draw(rec);
        }
        Drawothers();
        window.display();
        Sleep(NukeBeeingSpeed);
    }

    std::set<std::pair<int,int>> boomedOj;  //以防爆炸交集重複計算
    for(auto c : backup){
        for(int i=-1; i<=1; i++){   //y
            for(int j=-1; j<=1; j++){
                if(i==0 && j==0) continue;
                if(vis[(c.second+i)*(rows+2)+c.first+j]!=-2){
                    boomedOj.insert({c.first+j, c.second+i});
                }
            }
        }
    }
    //boom and boom animation // collect other activated nukes

    //boom sound effect
    sd.setBuffer(boom_sound);
    sd.play();

    //九宮格形狀
    window.clear(sf::Color(169,169,169));
    for(auto p : boomedOj){
        if(vis[(p.second)*(rows+2)+p.first]==6){    //activated other nuke

            //nuke ini sound effect
            sd.setBuffer(nk_ini_sound);
            sd.play();

            AcNkbank.push_back(p);  //初始登入方法
            rec.setTexture(textures[7]);
            rec.setPosition({origin_x + (p.first)*size_of_brick, origin_y + (p.second)*size_of_brick});
            window.draw(rec);
        }else{ //boom
            if(vis[(p.second)*(rows+2)+p.first]!=-1) SignoutOj(vis[(p.second)*(rows+2)+p.first], {p.first, p.second});
            rec.setTexture(textures[8]);
            rec.setPosition({origin_x + (p.first)*size_of_brick, origin_y + (p.second)*size_of_brick});
            window.draw(rec);
        }
    }
    Drawothers();
    window.display();
    Sleep(NukeWhiteSpeed);


    return (NukeEffect() || 1);
   return 0;
}
//登入一個剛被啟動的 NUKE 只需將其推入 bank 即可，因為它在anchor時已經有在vis , blockcount中 //每次進入nukeeffect，會將bank中的全部登出

void GiveFeedback(bool rpt){
    int sz = lineToClean.size();
    lines+=sz;
    if(sz == 1){
        if(t_spin) feedback = 5;
        else feedback = 0;
        combo++;
        tetrisB2B = 0;
    }else if(sz == 2){
        if(t_spin) feedback = 6;
        else feedback = 1;
        combo++;
        tetrisB2B = 0;
    }else if(sz == 3){
        if(t_spin) feedback = 7;
        else feedback = 2;
        combo++;
        tetrisB2B = 0;
    }else if(sz == 4){
        feedback = 3;
        combo++;
        tetrisB2B++;
    }else if(!rpt){
        combo = 0;
    }
    float Scscale = (combo > 1 ? pow(1.2, combo-1) : 1);
    if(feedback == 0){
        score += 100*Scscale;
    }
    else if(feedback == 1){
        score += 300*Scscale;
    }
    else if(feedback == 2){
        score += 600*Scscale;
    }
    else if(feedback == 3){
        score +=1000*Scscale*(tetrisB2B > 1 ? pow(1.4, tetrisB2B-1) : 1);
    }
    else if(feedback == 4){
        score += 30;
    }
    else if(feedback == 5){
        score += 300*Scscale;
    }
    else if(feedback == 6){
        score += 1000*Scscale;
    }
    else if(feedback == 7){
        score += 1500*Scscale;
    }
}

void DrawLosearea(){
    sf::RectangleShape rc({size_of_brick, size_of_brick});
    rc.setFillColor(sf::Color(0,100 ,100, 100));
    for(int i=1; i<=4; i++){
        for(int j=4; j<=7; j++){
            rc.setPosition({origin_x + j*size_of_brick, origin_y + i*size_of_brick});
            window.draw(rc);
        }
    }
}
void DrawLattice(){
    //lattice's line
        //vertical
    sf::RectangleShape line({line_thickness,colums*size_of_brick}); 
    line.setOrigin({line_thickness/2.f,colums*size_of_brick/2.f});
    line.setFillColor(lattice_color);
    for(int i=0; i<=rows; i++){
        line.setPosition({origin_x + size_of_brick*(i+1), origin_y + 15*size_of_brick});
        window.draw(line);
    }
        //horizontal
    line.setSize({rows*size_of_brick, line_thickness});
    line.setOrigin({rows*size_of_brick/2.f, line_thickness/2.f});
    for(int i=0; i<=colums; i++){
        line.setPosition({origin_x + 6*size_of_brick, origin_y + (5+i)*size_of_brick});    //y need to go down a brick
        window.draw(line);
    }
}
void DrawDroppedOj(){  //MyObject that has been set in vis
    for(int i = 0; i <= colums+4 ; i++){
        for(int j=0; j <=rows ; j++){
            int its_kind = vis[(i)*(rows+2)+(j)];
            if(its_kind == -2 || its_kind == -1) continue;
            rec.setTexture(textures[its_kind]);
            rec.setPosition({origin_x + size_of_brick*j, origin_y + size_of_brick*i});
            window.draw(rec);
        }
    }
}
void DrawMovingOj(){  //curObj, current shape and its shadow
    auto oj = curObj->getBrick();
    curObj->caculateDownShadow();
    auto shd = curObj->getShadow();
    int its_tx = (Gamemode == 0 ? curObj->getKind() : curObj->getShape());
    rec.setTexture(textures[its_tx]);
    //draw MyObject
    for(auto p : oj){
        int its_x = p.first;
        int its_y = p.second;
        rec.setPosition({origin_x+size_of_brick*its_x, origin_y+size_of_brick*its_y});
        window.draw(rec);
    }
    //draw shadow
    for(auto p : shd){
        int its_x = p.first;
        int its_y = p.second;
        Shadow_rec.setPosition({origin_x+size_of_brick*its_x, origin_y+size_of_brick*its_y});
        window.draw(Shadow_rec);
    }
}
void DrawNextOj(){ //next Obj, nxOb.front()
    std::stack<std::pair<int,int>> tmp_s;
    for(int i=0; i<nextAmo; i++){
        auto p = nxOb.front();
        MyObject nextOj(p.first, p.second, rows, colums, vis);
        int its_tx = (Gamemode == 0 ? p.second : p.first);
        rec.setTexture(textures[its_tx]);

        auto vc = nextOj.getBrick();
        for(auto g : vc){
            rec.setPosition({origin_x + (rows + 5 + g.first)*size_of_brick
                , origin_y + (g.second+ i*(colums/4))*size_of_brick});
            window.draw(rec);
        }
        tmp_s.push(p);
        nxOb.pop_front();
    }
    for(int i=0; i<nextAmo; i++){
        nxOb.push_front(tmp_s.top());
        tmp_s.pop();
    }
}
void DrawHoldOj(){ //draw held MyObject
    if(holdInfo.first == -1) return;
    MyObject holdOj(holdInfo.first, holdInfo.second, rows, colums, vis);
    int its_tx = (Gamemode == 0 ? holdInfo.second : holdInfo.first);
    rec.setTexture(textures[its_tx]);
    auto vc = holdOj.getBrick();
    for(auto p : vc){
        rec.setPosition({origin_x + (-9 + p.first)*size_of_brick, origin_y + (-2 + p.second)*size_of_brick});
        window.draw(rec);
    }
}
void PrintText(){  //hold, lines, score
    //Hold
    sf::Text text(fonts[0]);
    text.setOrigin({0.f, 0.f});
    text.setString("Hold");    //set string
    text.setCharacterSize(50); //size in pixels
    text.setFillColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    text.setPosition({origin_x - size_of_brick*4.65f, origin_y - size_of_brick*4.75f});
    window.draw(text);
    //Lines
    text.setString("Lines");
    text.setPosition({origin_x + (rows+14.f)*size_of_brick, origin_y - size_of_brick*3.f});
    window.draw(text);
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);
    text.setPosition({origin_x + (rows+14.f)*size_of_brick, origin_y});
    text.setString(std::to_string(lines));
    window.draw(text);
    //Score
    text.setString("Score : ");
    text.setPosition({origin_x + (rows+14.f)*size_of_brick, origin_y + size_of_brick*12.f});
    window.draw(text);
    text.setCharacterSize(70);
    text.setPosition({origin_x + (rows+14.f)*size_of_brick, origin_y + size_of_brick*17.f});
    text.setString(std::to_string(score));
    window.draw(text);
}
void PrintFeedback(){
    //the attck
    sf::Text text(fonts[0]);
    text.setCharacterSize(60);
    text.setFillColor(sf::Color(0,0,255,alpha));
    text.setPosition({origin_x - 10*text.getCharacterSize(), origin_y + size_of_brick*5});
    std::string s = " ";
    if(feedback == 0) s = "SINGLE";
    else if(feedback == 1) s = "DOUBLE";
    else if(feedback == 2) s = "TRIPLE";
    else if(feedback == 3) s = "TETRIS";
    else if(feedback == 4) s = "T-SPIN MINI";
    else if(feedback == 5) s = "T-SPIN SINGLE";
    else if(feedback == 6) s = "T-SPIN DOUBLE";
    else if(feedback == 7) s = "T-SPIN TRIPLE";
    text.setString(s);
    window.draw(text);

    //TETRIS B2B
    if(tetrisB2B > 1){
        text.setFillColor(sf::Color(255,0,0));
        text.setPosition({origin_x - 10*text.getCharacterSize(), origin_y + size_of_brick*8});
        s = "B2B X"; 
        s.append(std::to_string(tetrisB2B-1));
        text.setString(s);
        window.draw(text);
    }
    //COMBO
    if(combo > 1){
        text.setFillColor(sf::Color(0,255,0));
        text.setPosition({origin_x - 10*text.getCharacterSize(), origin_y + size_of_brick*10});
        s = "COMBO X";
        s.append(std::to_string(combo-1));
        text.setString(s);
        window.draw(text);
    }
}
void Drawothers(){  //draw all the stuff but moving shape and its shadow
    PrintText();
    PrintFeedback();
    DrawLattice();
    DrawHoldOj();
    DrawNextOj();
    DrawDroppedOj();    //vis
    DrawLosearea();
}

void PrintLoseState(){
    window.clear(sf::Color(169,169,169));
    sf::Text losss(fonts[0]);
    losss.setString(" Lose ");
    losss.setCharacterSize(std::min(window_h, window_w) / 20);
    losss.setOrigin({(float)3*losss.getCharacterSize(), (float)losss.getCharacterSize()/2});
    losss.setPosition({(float)window_w/2, (float)window_h/2});
    window.draw(losss);
    losss.setString(" Press R to restart ");
    losss.setOrigin({(float)5*losss.getCharacterSize(), losss.getCharacterSize()/2.f});
    losss.setPosition({(float)window_w/2, window_h/2  + (float)losss.getCharacterSize()});
    window.draw(losss);
    window.display();
}
void defTextures(){
    textures.resize(9);
    if(Gamemode == 0){
        textures = 
        {
            sf::Texture("tiles_S.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("Sand.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("Blackhole.jpg", false, sf::IntRect({0,0},{472,472})),
            sf::Texture("Boson.jpg", false, sf::IntRect({0,0},{472,472})),
            sf::Texture("Mower.jpg", false, sf::IntRect({0,0},{472,472})),
            sf::Texture("Classmate.jpg", false, sf::IntRect({0,0}, {480,472})),
            sf::Texture("Nuke sign.png", false, sf::IntRect({0,0}, {600,600})),
            sf::Texture("Activated Nuke.png", false, sf::IntRect({0,0}, {507,507})),  //activated nuke  //7
            sf::Texture("Boom image.jpg", false, sf::IntRect({0,0}, {416,416}))  //Boom of nuke //8
        };
    }else if(Gamemode == 1){
        textures = 
        {
            sf::Texture("tiles_S.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_Z.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_T.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_O.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_L.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_J.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_I.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_S.jpg", false, sf::IntRect({0,0}, {472,472})),
            sf::Texture("tiles_S.jpg", false, sf::IntRect({0,0}, {472,472})),
        };
    }
}
