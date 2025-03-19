//-------------------------------------
//控制方塊在所傳入的陣列座標中移動的標頭檔
//-------------------------------------

#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

#ifndef MYOBJECT_H
#define MYOBJECT_H
class MyObject{
    private:
    int rows;                               //地圖的長
    int colums;                             //地圖的寬
    int* vis;                               //地圖(已拜訪狀態做出合法操作)
    int shape;                              //方塊的形狀
    int kind;                               //方塊的功能類型
    int cur_spin;                           //目前方塊的旋轉狀態
    int pre_spin;                           //上次方塊的旋轉狀態
    std::pair<float, float> center;         //方塊目前的中心座標
    std::pair<float, float> shadowCenter;   //方塊影子的中心座標
    void initiating(int shape, int rows, int colums, int* vis);
    
    protected:
    

    public:
    //  Constructor
    //初始化，賦予每個變數值
    MyObject (int shape, int kind, int rows, int colums, int* vis);
    MyObject (){};
    virtual ~MyObject();
    int move(int dx, int dy);                           //以vis判斷位移是否合法後做出位移(更改方塊中心座標)，
                                                        //若為下移則回傳不合法次數(達到特定次數直接固定方塊)
                                                        
    bool spin(int d);                                   //以vis判斷旋轉是否合法後做出旋轉(更改旋轉方向)
    void setShape(int shape);                           //更改方塊形狀
    void setKind( int kind );                           //更改方塊功能種類
    int getShape();                                     //回傳目前的方塊形狀
    int getKind();                                      //回傳目前的方塊功能種類
    void setShadowCenter(std::pair<float,float>);       //手動調整影子中心
    std::pair<float, float> getShadowCenter(){          //回傳目前的影子中心
        return this->shadowCenter;
    }
    std::vector<std::pair<float,float>> getBrick();     //以目前的中心為準，給出整個方塊的座標
    std::vector<std::pair<float,float>> getShadow();    //以目前的影子中心為準，給出整個影子座標
    void caculateDownShadow();                          //以目前的方塊中心為準，算出正下方的影子座標，並以其賦值shadow center
};
#endif
