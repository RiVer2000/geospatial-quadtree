// #include <iostream>
// #include <vector>
// #include <opencv2/opencv.hpp>

// struct Point {
//     int x, y;
//     Point(int _x, int _y) : x(_x), y(_y) {}
//     Point() : x(0), y(0) {}
// };

// struct Node {
//     Point pos;
//     int width, height;
//     cv::Mat pixels;

//     Node(Point _pos, int _width, int _height, cv::Mat _pixels)
//         : pos(_pos), width(_width), height(_height), pixels(_pixels) {}
// };

// class Quad {
//     Point topLeft, botRight;
//     Node* n;
//     Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;

// public:
//     Quad(Point topL, Point botR, const cv::Mat& image) {
//         topLeft = topL;
//         botRight = botR;
//         n = nullptr;
//         topLeftTree = nullptr;
//         topRightTree = nullptr;
//         botLeftTree = nullptr;
//         botRightTree = nullptr;

//         cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
//         cv::Mat region = image(roi).clone();

//         n = new Node(topLeft, roi.width, roi.height, region);

//         if (roi.width > 40 || roi.height > 40) {
//             subdivide(image);
//         }
//     }

//     void subdivide(const cv::Mat& image);
//     Quad* findSmallestCommonAncestor(Point p1, Point p2);
//     bool inBoundary(Point p);
//     cv::Mat extractRegion(Point p1, Point p2);
// };

// void Quad::subdivide(const cv::Mat& image) {
//     int midX = (topLeft.x + botRight.x) / 2;
//     int midY = (topLeft.y + botRight.y) / 2;

//     if (botRight.x - topLeft.x <= 40 && botRight.y - topLeft.y <= 40)
//         return;

//     if (!topLeftTree)
//         topLeftTree = new Quad(topLeft, Point(midX, midY), image);
    
//     if (!topRightTree)
//         topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY), image);
    
//     if (!botLeftTree)
//         botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y), image);
    
//     if (!botRightTree)
//         botRightTree = new Quad(Point(midX, midY), botRight, image);
// }

// Quad* Quad::findSmallestCommonAncestor(Point p1, Point p2) {
//     if (!inBoundary(p1) || !inBoundary(p2)) return nullptr;
//     if (!topLeftTree) return this;

//     bool tl = topLeftTree && topLeftTree->inBoundary(p1) && topLeftTree->inBoundary(p2);
//     bool tr = topRightTree && topRightTree->inBoundary(p1) && topRightTree->inBoundary(p2);
//     bool bl = botLeftTree && botLeftTree->inBoundary(p1) && botLeftTree->inBoundary(p2);
//     bool br = botRightTree && botRightTree->inBoundary(p1) && botRightTree->inBoundary(p2);

//     if (tl) return topLeftTree->findSmallestCommonAncestor(p1, p2);
//     if (tr) return topRightTree->findSmallestCommonAncestor(p1, p2);
//     if (bl) return botLeftTree->findSmallestCommonAncestor(p1, p2);
//     if (br) return botRightTree->findSmallestCommonAncestor(p1, p2);

//     return this;
// }

// cv::Mat Quad::extractRegion(Point p1, Point p2) {
//     Quad* ancestor = findSmallestCommonAncestor(p1, p2);
//     if (!ancestor) return cv::Mat();
//     return ancestor->n->pixels.clone();
// }

// bool Quad::inBoundary(Point p) {
//     return (p.x >= topLeft.x && p.x <= botRight.x
//             && p.y >= topLeft.y && p.y <= botRight.y);
// }

// int main() {
//     cv::Mat image = cv::imread("pikachu.jpeg"); // Load an image
//     if (image.empty()) {
//         std::cerr << "Error: Unable to load image!" << std::endl;
//         return -1;
//     }

//     Quad quadtree(Point(0, 0), Point(image.cols, image.rows), image);

//     Point p1(1800, 100);
//     Point p2(1811, 210);
//     cv::Mat region = quadtree.extractRegion(p1, p2);

//     if (!region.empty()) {
//         cv::imshow("Extracted Region", region);
//         cv::waitKey(0);
//     } else {
//         std::cout << "No region found!" << std::endl;
//     }

//     return 0;
// }

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
    Point() : x(0), y(0) {}
};

struct Node {
    Point pos;
    int width, height;
    cv::Mat pixels;

    Node(Point _pos, int _width, int _height, cv::Mat _pixels)
        : pos(_pos), width(_width), height(_height), pixels(_pixels) {}
};

class Quad {
    Point topLeft, botRight;
    Node* n;
    Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;
    bool isLoaded;

public:
    Quad(Point topL, Point botR) {
        topLeft = topL;
        botRight = botR;
        n = nullptr;
        topLeftTree = nullptr;
        topRightTree = nullptr;
        botLeftTree = nullptr;
        botRightTree = nullptr;
        isLoaded = false;
    }

    void load(const cv::Mat& image);
    void subdivide(const cv::Mat& image);
    Quad* findSmallestCommonAncestor(Point p1, Point p2);
    bool inBoundary(Point p);
    cv::Mat extractRegion(Point p1, Point p2, const cv::Mat& image);
};

void Quad::load(const cv::Mat& image) {
    if (isLoaded) return;

    cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
    cv::Mat region = image(roi).clone();

    n = new Node(topLeft, roi.width, roi.height, region);
    isLoaded = true;
}

void Quad::subdivide(const cv::Mat& image) {
    if (botRight.x - topLeft.x <= 5 && botRight.y - topLeft.y <= 5)
        return; // More fine-grained subdivisions

    int midX = (topLeft.x + botRight.x) / 2;
    int midY = (topLeft.y + botRight.y) / 2;

    if (!topLeftTree) topLeftTree = new Quad(topLeft, Point(midX, midY));
    if (!topRightTree) topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY));
    if (!botLeftTree) botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y));
    if (!botRightTree) botRightTree = new Quad(Point(midX, midY), botRight);

    topLeftTree->subdivide(image);
    topRightTree->subdivide(image);
    botLeftTree->subdivide(image);
    botRightTree->subdivide(image);
}

Quad* Quad::findSmallestCommonAncestor(Point p1, Point p2) {
    if (!inBoundary(p1) || !inBoundary(p2)) return nullptr;
    if (!topLeftTree) return this; // Ensure we go as deep as possible

    bool tl = topLeftTree && topLeftTree->inBoundary(p1) && topLeftTree->inBoundary(p2);
    bool tr = topRightTree && topRightTree->inBoundary(p1) && topRightTree->inBoundary(p2);
    bool bl = botLeftTree && botLeftTree->inBoundary(p1) && botLeftTree->inBoundary(p2);
    bool br = botRightTree && botRightTree->inBoundary(p1) && botRightTree->inBoundary(p2);

    if (tl) return topLeftTree->findSmallestCommonAncestor(p1, p2);
    if (tr) return topRightTree->findSmallestCommonAncestor(p1, p2);
    if (bl) return botLeftTree->findSmallestCommonAncestor(p1, p2);
    if (br) return botRightTree->findSmallestCommonAncestor(p1, p2);

    return this;
}

cv::Mat Quad::extractRegion(Point p1, Point p2, const cv::Mat& image) {
    Quad* ancestor = findSmallestCommonAncestor(p1, p2);
    if (!ancestor) return cv::Mat();

    if (!ancestor->isLoaded) {
        ancestor->load(image);
    }
    
    return ancestor->n->pixels.clone();
}

bool Quad::inBoundary(Point p) {
    return (p.x >= topLeft.x && p.x <= botRight.x
            && p.y >= topLeft.y && p.y <= botRight.y);
}

int main() {
    cv::Mat image = cv::imread("pikachu.jpeg"); // Load an image
    if (image.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    Quad quadtree(Point(0, 0), Point(image.cols, image.rows));
    quadtree.subdivide(image); // Ensure quadtree is pre-subdivided properly

    Point p1(1800, 100);
    Point p2(1800, 111);
    cv::Mat region = quadtree.extractRegion(p1, p2, image);

    if (!region.empty()) {
        cv::imshow("Extracted Region", region);
        cv::waitKey(0);
    } else {
        std::cout << "No region found!" << std::endl;
    }

    return 0;
}