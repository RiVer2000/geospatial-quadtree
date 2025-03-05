// #include <iostream>
// #include <vector>
// #include <unordered_map>
// #include <opencv2/opencv.hpp>

// struct Point {
//     int x, y;
//     Point(int _x, int _y) : x(_x), y(_y) {}
//     Point() : x(0), y(0) {}
// };

// struct Node {
//     Point pos;
//     int width, height;
//     cv::Rect roi; // Instead of storing full image, store only ROI coordinates

//     Node(Point _pos, int _width, int _height, cv::Rect _roi)
//         : pos(_pos), width(_width), height(_height), roi(_roi) {}
// };

// class Quad {
//     Point topLeft, botRight;
//     Node* n;
//     Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;
//     bool isLoaded;
//     static std::unordered_map<std::string, cv::Mat> cache; // Simple caching mechanism

// public:
//     Quad(Point topL, Point botR) {
//         topLeft = topL;
//         botRight = botR;
//         n = nullptr;
//         topLeftTree = nullptr;
//         topRightTree = nullptr;
//         botLeftTree = nullptr;
//         botRightTree = nullptr;
//         isLoaded = false;
//     }

//     void load(const cv::Mat& image);
//     void subdivide(const cv::Mat& image);
//     Quad* findSmallestCommonAncestor(Point p1, Point p2);
//     bool inBoundary(Point p);
//     Quad* getSmallestLeafContaining(Point p);
//     cv::Mat extractRegion(Point p1, Point p2, const cv::Mat& image);
// };

// std::unordered_map<std::string, cv::Mat> Quad::cache;

// void Quad::load(const cv::Mat& image) {
//     if (isLoaded) return;
    
//     cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
//     std::string cacheKey = std::to_string(topLeft.x) + "_" + std::to_string(topLeft.y) + "_" + std::to_string(botRight.x) + "_" + std::to_string(botRight.y);
    
//     if (cache.find(cacheKey) == cache.end()) {
//         cache[cacheKey] = image(roi).clone();
//     }
    
//     n = new Node(topLeft, roi.width, roi.height, roi);
//     isLoaded = true;
// }

// void Quad::subdivide(const cv::Mat& image) {
//     if (botRight.x - topLeft.x <= 2 && botRight.y - topLeft.y <= 2)
//         return;

//     int midX = (topLeft.x + botRight.x) / 2;
//     int midY = (topLeft.y + botRight.y) / 2;

//     if (!topLeftTree) topLeftTree = new Quad(topLeft, Point(midX, midY));
//     if (!topRightTree) topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY));
//     if (!botLeftTree) botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y));
//     if (!botRightTree) botRightTree = new Quad(Point(midX, midY), botRight);

//     topLeftTree->subdivide(image);
//     topRightTree->subdivide(image);
//     botLeftTree->subdivide(image);
//     botRightTree->subdivide(image);
// }

// Quad* Quad::findSmallestCommonAncestor(Point p1, Point p2) {
//     if (!inBoundary(p1) || !inBoundary(p2)) return nullptr;
//     if (!topLeftTree) return this; // If we are at a leaf, return

//     if (topLeftTree->inBoundary(p1) && topLeftTree->inBoundary(p2)) return topLeftTree->findSmallestCommonAncestor(p1, p2);
//     if (topRightTree->inBoundary(p1) && topRightTree->inBoundary(p2)) return topRightTree->findSmallestCommonAncestor(p1, p2);
//     if (botLeftTree->inBoundary(p1) && botLeftTree->inBoundary(p2)) return botLeftTree->findSmallestCommonAncestor(p1, p2);
//     if (botRightTree->inBoundary(p1) && botRightTree->inBoundary(p2)) return botRightTree->findSmallestCommonAncestor(p1, p2);

//     return this;
// }

// cv::Mat Quad::extractRegion(Point p1, Point p2, const cv::Mat& image) {
//     Quad* ancestor = findSmallestCommonAncestor(p1, p2);
//     if (!ancestor) return cv::Mat();

//     if (!ancestor->isLoaded) {
//         ancestor->load(image);
//     }
    
//     std::string cacheKey = std::to_string(ancestor->topLeft.x) + "_" + std::to_string(ancestor->topLeft.y) + "_" + std::to_string(ancestor->botRight.x) + "_" + std::to_string(ancestor->botRight.y);
//     return cache[cacheKey].clone();
// }

// bool Quad::inBoundary(Point p) {
//     return (p.x >= topLeft.x && p.x <= botRight.x
//             && p.y >= topLeft.y && p.y <= botRight.y);
// }

// int main() {
//     cv::Mat image = cv::imread("pikachu.jpeg");
//     if (image.empty()) {
//         std::cerr << "Error: Unable to load image!" << std::endl;
//         return -1;
//     }

//     Quad quadtree(Point(0, 0), Point(image.cols, image.rows));
//     quadtree.subdivide(image);

//     Point p1(1800, 1080);
//     Point p2(1800, 114);
//     cv::Mat region = quadtree.extractRegion(p1, p2, image);

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
#include <unordered_map>
#include <list>
#include <opencv2/opencv.hpp>

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
    Point() : x(0), y(0) {}
};

struct Node {
    Point pos;
    int width, height;
    cv::Rect roi; // Store only ROI coordinates

    Node(Point _pos, int _width, int _height, cv::Rect _roi)
        : pos(_pos), width(_width), height(_height), roi(_roi) {}
};

class LRUCache {
    int capacity;
    std::list<std::string> order;
    std::unordered_map<std::string, std::pair<cv::Mat, std::list<std::string>::iterator>> cache;

public:
    LRUCache(int cap) : capacity(cap) {}

    cv::Mat get(const std::string& key) {
        if (cache.find(key) == cache.end()) return cv::Mat();
        order.splice(order.begin(), order, cache[key].second);
        return cache[key].first;
    }

    void put(const std::string& key, const cv::Mat& value) {
        if (cache.find(key) != cache.end()) {
            order.splice(order.begin(), order, cache[key].second);
            cache[key].first = value;
            return;
        }

        if (cache.size() >= capacity) {
            std::string last = order.back();
            order.pop_back();
            cache.erase(last);
        }

        order.push_front(key);
        cache[key] = {value, order.begin()};
    }
};

class Quad {
    Point topLeft, botRight;
    Node* n;
    Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;
    bool isLoaded;
    static LRUCache cache;

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

LRUCache Quad::cache(10); // LRU cache with 10 regions max

void Quad::load(const cv::Mat& image) {
    if (isLoaded) return;
    
    cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
    std::string cacheKey = std::to_string(topLeft.x) + "_" + std::to_string(topLeft.y) + "_" + std::to_string(botRight.x) + "_" + std::to_string(botRight.y);
    
    cv::Mat cachedRegion = cache.get(cacheKey);
    if (cachedRegion.empty()) {
        cv::Mat newRegion = image(roi).clone();
        cache.put(cacheKey, newRegion);
    }
    
    n = new Node(topLeft, roi.width, roi.height, roi);
    isLoaded = true;
}

void Quad::subdivide(const cv::Mat& image) {
    if (botRight.x - topLeft.x <= 2 && botRight.y - topLeft.y <= 2)
        return;

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
    if (!topLeftTree) return this;

    if (topLeftTree->inBoundary(p1) && topLeftTree->inBoundary(p2)) return topLeftTree->findSmallestCommonAncestor(p1, p2);
    if (topRightTree->inBoundary(p1) && topRightTree->inBoundary(p2)) return topRightTree->findSmallestCommonAncestor(p1, p2);
    if (botLeftTree->inBoundary(p1) && botLeftTree->inBoundary(p2)) return botLeftTree->findSmallestCommonAncestor(p1, p2);
    if (botRightTree->inBoundary(p1) && botRightTree->inBoundary(p2)) return botRightTree->findSmallestCommonAncestor(p1, p2);

    return this;
}

cv::Mat Quad::extractRegion(Point p1, Point p2, const cv::Mat& image) {
    Quad* ancestor = findSmallestCommonAncestor(p1, p2);
    if (!ancestor) return cv::Mat();

    if (!ancestor->isLoaded) {
        ancestor->load(image);
    }
    
    std::string cacheKey = std::to_string(ancestor->topLeft.x) + "_" + std::to_string(ancestor->topLeft.y) + "_" + std::to_string(ancestor->botRight.x) + "_" + std::to_string(ancestor->botRight.y);
    return cache.get(cacheKey);
}

bool Quad::inBoundary(Point p) {
    return (p.x >= topLeft.x && p.x <= botRight.x
            && p.y >= topLeft.y && p.y <= botRight.y);
}

int main() {
    cv::Mat image = cv::imread("pikachu.jpeg");
    if (image.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    Quad quadtree(Point(0, 0), Point(image.cols, image.rows));
    quadtree.subdivide(image);

    Point p1(1800, 100);
    Point p2(1800, 114);
    cv::Mat region = quadtree.extractRegion(p1, p2, image);

    if (!region.empty()) {
        cv::imshow("Extracted Region", region);
        cv::waitKey(0);
    } else {
        std::cout << "No region found!" << std::endl;
    }

    // Save the region to disk to a folder extracted_map 
    cv::imwrite("extracted_map/region.png", region);

    return 0;
}
