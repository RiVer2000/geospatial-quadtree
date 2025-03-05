#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp> // Include OpenCV

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
    Point() : x(0), y(0) {}
};

struct Node {
    Point pos;
    int width, height;
    cv::Mat pixels; // Store full-color image

    Node(Point _pos, int _width, int _height, cv::Mat _pixels)
        : pos(_pos), width(_width), height(_height), pixels(_pixels) {}
};

class Quad {
    Point topLeft, botRight;
    Node* n;
    Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;

public:
    Quad(Point topL, Point botR, const cv::Mat& image) {
        topLeft = topL;
        botRight = botR;
        n = nullptr;
        topLeftTree = nullptr;
        topRightTree = nullptr;
        botLeftTree = nullptr;
        botRightTree = nullptr;

        // Extract the correct image region (RGB)
        cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
        cv::Mat region = image(roi).clone();

        n = new Node(topLeft, roi.width, roi.height, region);

        if (roi.width > 40 || roi.height > 40) {
            subdivide(image);
        }
    }

    void subdivide(const cv::Mat& image);
    cv::Mat searchRegion(Point p1, Point p2);
    bool inBoundary(Point p);
};

void Quad::subdivide(const cv::Mat& image) {
    int midX = (topLeft.x + botRight.x) / 2;
    int midY = (topLeft.y + botRight.y) / 2;

    if (botRight.x - topLeft.x <= 40 && botRight.y - topLeft.y <= 40)
        return; // Stop subdividing at 40x40

    if (!topLeftTree)
        topLeftTree = new Quad(topLeft, Point(midX, midY), image);
    
    if (!topRightTree)
        topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY), image);
    
    if (!botLeftTree)
        botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y), image);
    
    if (!botRightTree)
        botRightTree = new Quad(Point(midX, midY), botRight, image);
}


// cv::Mat Quad::searchRegion(Point p1, Point p2) {
//     // Ensure both points are within the image boundary
//     if (!inBoundary(p1) || !inBoundary(p2))
//         return cv::Mat(); // Return empty if either point is outside

//     // Compute the smallest bounding box that contains both points
//     int minX = std::min(p1.x, p2.x);
//     int maxX = std::max(p1.x, p2.x);
//     int minY = std::min(p1.y, p2.y);
//     int maxY = std::max(p1.y, p2.y);

//     // Ensure bounding box does not exceed image size
//     minX = std::max(0, minX);
//     minY = std::max(0, minY);
//     maxX = std::min(botRight.x, maxX);
//     maxY = std::min(botRight.y, maxY);

//     // Extract the exact submap from the image
//     cv::Rect roi(minX, minY, maxX - minX, maxY - minY);
//     return n->pixels(roi).clone();
// }

cv::Mat Quad::searchRegion(Point p1, Point p2) {
    // Ensure both points are within the image boundary
    if (!inBoundary(p1) || !inBoundary(p2))
        return cv::Mat(); // Return empty if either point is outside

    // Compute the smallest bounding box that contains both points
    int minX = std::min(p1.x, p2.x);
    int maxX = std::max(p1.x, p2.x);
    int minY = std::min(p1.y, p2.y);
    int maxY = std::max(p1.y, p2.y);

    // Ensure bounding box does not exceed image size
    minX = std::max(0, minX);
    minY = std::max(0, minY);
    maxX = std::min(botRight.x, maxX);
    maxY = std::min(botRight.y, maxY);

    // 🔥 Fix for single-row or single-column selections
    int width = std::max(1, maxX - minX);  // Ensure at least 1 pixel width
    int height = std::max(1, maxY - minY); // Ensure at least 1 pixel height

    // Extract the exact submap from the image
    cv::Rect roi(minX, minY, width, height);
    return n->pixels(roi).clone();
}



bool Quad::inBoundary(Point p) {
    return (p.x >= topLeft.x && p.x <= botRight.x
            && p.y >= topLeft.y && p.y <= botRight.y);
}

// int main() {
//     // Load an RGB image
//     cv::Mat image = cv::imread("pikachu.jpeg"); // Replace "image.jpg" with your image file
//     if (image.empty()) {
//         std::cerr << "Error: Unable to load image!" << std::endl;
//         return -1;
//     }

//     // Create the quadtree
//     Quad quadtree(Point(0, 0), Point(image.cols, image.rows), image);

//     // Define two points
//     Point p1(1800, 100);
//     Point p2(900, 300);

//     // Find the smallest region containing both points
//     cv::Mat region = quadtree.searchRegion(p1, p2);

//     if (!region.empty()) {
//         // Show the extracted submap
//         cv::imshow("Extracted Submap", region);
//         cv::waitKey(0); // Wait for user to close window
//     } else {
//         std::cout << "No region found!" << std::endl;
//     }

//     return 0;
// }

int main() {
    // Load an RGB image
    cv::Mat image = cv::imread("pikachu.jpeg"); 
    if (image.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Create the quadtree
    Quad quadtree(Point(0, 0), Point(image.cols, image.rows), image);

    // Define two points
    Point p1(110, 400);
    Point p2(1800, 712);

    // Extract the exact bounding box
    cv::Mat region = quadtree.searchRegion(p1, p2);

    if (!region.empty()) {
        // Show the extracted submap
        cv::imshow("Extracted Submap", region);
        cv::waitKey(0); // Wait for user to close window
    } else {
        std::cout << "No region found!" << std::endl;
    }

    return 0;
}

