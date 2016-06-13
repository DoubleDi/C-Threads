#include<iostream>
using namespace std;

class Rectangle {
     public:
     int width, height;
     Rectangle() {};
     Rectangle(int x, int y) :  width(x), height(y) { };
     int s() { return width * height; };
     int m() { return max(width, height); };
     Rectangle operator+ (Rectangle &rect) {
         Rectangle tmp;
         tmp.width = width + rect.width;
         tmp.height = height + rect.height;
         return tmp;
     }
     ~Rectangle () { cout << "\nFINISH!\n";};
};

class PRect {
   Rectangle * rect;
   public:
   PRect() : rect(NULL) {}
   PRect(Rectangle * r): rect(r) {}
   operator Rectangle*() { return rect; }
   Rectangle* operator->() { return rect;}

};

int
main(void)
{
    Rectangle rect1 (1, 2);
    Rectangle rect2 (3, 4);
    Rectangle rect3 = rect1 + rect2;
    cout << rect1.s() << " " << rect2.m() << " " << rect3.width << " " << rect3.height << "\n";
    //void f(Rectangle *);
    PRect prect1(&rect1);
    //f(prect1);
    cout << prect1->width;
}
