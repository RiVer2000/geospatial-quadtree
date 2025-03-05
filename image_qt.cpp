// #include <cmath>
// #include <iostream>
// #include <vector>

// struct Point
// {
//     int x, y;
//     Point(int _x, int _y) : x(_x), y(_y) {}
//     Point() : x(0), y(0) {}
// };

// struct Node {
//     Point pos;       // Position of the region (top-left)
//     int width, height;  // Dimensions of the region
//     std::vector<std::vector<int>> pixels; // Stores pixel data (simplified as an integer for grayscale)
    
//     Node(Point _pos, int _width, int _height, std::vector<std::vector<int>> _pixels)
//         : pos(_pos), width(_width), height(_height), pixels(_pixels) {}
// };

// class Quad {
//     Point topLeft, botRight;
//     Node* n; // Stores the image section
//     Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;

// public:
//     Quad(Point topL, Point botR, std::vector<std::vector<int>>& image) {
//         topLeft = topL;
//         botRight = botR;
//         n = NULL;
//         topLeftTree = NULL;
//         topRightTree = NULL;
//         botLeftTree = NULL;
//         botRightTree = NULL;

//         // Extract the image region and store it
//         int width = botRight.x - topLeft.x;
//         int height = botRight.y - topLeft.y;
//         std::vector<std::vector<int>> region(height, std::vector<int>(width));

//         for (int i = 0; i < height; ++i)
//             for (int j = 0; j < width; ++j)
//                 region[i][j] = image[topLeft.y + i][topLeft.x + j];

//         n = new Node(topLeft, width, height, region);
//     }

//     void subdivide(); // Function to split further
//     std::vector<std::vector<int>> searchRegion(Point p, int size);
//     bool inBoundary(Point);
// };

// void Quad::subdivide() {
//     int midX = (topLeft.x + botRight.x) / 2;
//     int midY = (topLeft.y + botRight.y) / 2;

//     if (botRight.x - topLeft.x <= 40 && botRight.y - topLeft.y <= 40)
//         return; // Stop subdividing when reaching 40x40

//     if (!topLeftTree)
//         topLeftTree = new Quad(topLeft, Point(midX, midY), n->pixels);
    
//     if (!topRightTree)
//         topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY), n->pixels);
    
//     if (!botLeftTree)
//         botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y), n->pixels);
    
//     if (!botRightTree)
//         botRightTree = new Quad(Point(midX, midY), botRight, n->pixels);
// }

// std::vector<std::vector<int>> Quad::searchRegion(Point p, int size) {
//     if (!inBoundary(p))
//         return {}; // Return empty if out of bounds

//     if (botRight.x - topLeft.x <= size && botRight.y - topLeft.y <= size)
//         return n->pixels; // Return this region if it's small enough

//     if ((topLeft.x + botRight.x) / 2 >= p.x) {
//         if ((topLeft.y + botRight.y) / 2 >= p.y)
//             return topLeftTree ? topLeftTree->searchRegion(p, size) : std::vector<std::vector<int>>();
//         else
//             return botLeftTree ? botLeftTree->searchRegion(p, size) : std::vector<std::vector<int>>();
//     } else {
//         if ((topLeft.y + botRight.y) / 2 >= p.y)
//             return topRightTree ? topRightTree->searchRegion(p, size) : std::vector<std::vector<int>>();
//         else
//             return botRightTree ? botRightTree->searchRegion(p, size) : std::vector<std::vector<int>>();
//     }
// }

// // Check if current quadtree contains the point
// bool Quad::inBoundary(Point p)
// {
//     return (p.x >= topLeft.x && p.x <= botRight.x
//             && p.y >= topLeft.y && p.y <= botRight.y);
// }

// int main() {
//     // Example grayscale image (1920x1080) initialized with dummy values
//     std::vector<std::vector<int>> image(1080, std::vector<int>(1920, 255)); // White image

//     // Create the quadtree
//     Quad quadtree(Point(0, 0), Point(1920, 1080), image);

//     // Search for the 40x40 region around (12, 180)
//     std::vector<std::vector<int>> region = quadtree.searchRegion(Point(12, 180), 40);

//     // Print the region (for visualization)
//     std::cout << "40x40 Region centered at (12,180):\n";
//     for (const auto& row : region) {
//         for (int pixel : row)
//         std::cout << pixel << " ";
//         std::cout << "\n";
//     }

//     return 0;
// }


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
    cv::Mat pixels; // Use OpenCV Mat for image storage

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

        // Extract the correct image region
        cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
        cv::Mat region = image(roi).clone();

        n = new Node(topLeft, roi.width, roi.height, region);

        if (roi.width > 40 || roi.height > 40) {
            subdivide(image);
        }
    }

    void subdivide(const cv::Mat& image);
    cv::Mat searchRegion(Point p, int size);
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

cv::Mat Quad::searchRegion(Point p, int size) {
    if (!inBoundary(p))
        return cv::Mat(); // Return empty Mat if out of bounds

    if (botRight.x - topLeft.x <= size && botRight.y - topLeft.y <= size)
        return n->pixels; // Return this region if it's small enough

    if ((topLeft.x + botRight.x) / 2 >= p.x) {
        if ((topLeft.y + botRight.y) / 2 >= p.y)
            return topLeftTree ? topLeftTree->searchRegion(p, size) : cv::Mat();
        else
            return botLeftTree ? botLeftTree->searchRegion(p, size) : cv::Mat();
    } else {
        if ((topLeft.y + botRight.y) / 2 >= p.y)
            return topRightTree ? topRightTree->searchRegion(p, size) : cv::Mat();
        else
            return botRightTree ? botRightTree->searchRegion(p, size) : cv::Mat();
    }
}

bool Quad::inBoundary(Point p) {
    return (p.x >= topLeft.x && p.x <= botRight.x
            && p.y >= topLeft.y && p.y <= botRight.y);
}

int main() {
    // Load an image
    cv::Mat image = cv::imread("pikachu.jpeg"); // Replace "image.jpg" with your image file
    if (image.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Convert to grayscale (optional)
    // cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

    // Create the quadtree
    Quad quadtree(Point(0, 0), Point(image.cols, image.rows), image);

    // Search for the 40x40 region around (12,180)
    cv::Mat region = quadtree.searchRegion(Point(1200, 280), 300);

    if (!region.empty()) {
        // Show the extracted 40x40 region
        cv::imshow("Extracted 40x40 Region", region);
        cv::waitKey(0); // Wait for user to close window
    } else {
        std::cout << "No region found!" << std::endl;
    }

    return 0;
}

