#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <opencv2/opencv.hpp>
#include <chrono>

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
};

class Quad {
public:
    Point topLeft, botRight;
    Quad *topLeftTree, *topRightTree, *botLeftTree, *botRightTree;
    cv::Mat region;
    bool isLeaf;
    static int resolution;

    Quad(Point topL, Point botR);
    void subdivide();
    bool inBoundary(Point p);
    Quad* getLeafContaining(Point p);
    void loadRegion(const cv::Mat &image);
    void dynamicSubdivide(const cv::Mat &image, Point queryPoint);
    Quad* findSmallestCommonAncestor(Point p1, Point p2);
    cv::Mat extractRegionContainingPoints(Point p1, Point p2, const cv::Mat &image);
};

int Quad::resolution = 256; // Default resolution limit

Quad::Quad(Point topL, Point botR) : topLeft(topL), botRight(botR),
    topLeftTree(nullptr), topRightTree(nullptr),
    botLeftTree(nullptr), botRightTree(nullptr),
    isLeaf(true) {}

void Quad::subdivide() {
    if ((botRight.x - topLeft.x) <= resolution || (botRight.y - topLeft.y) <= resolution) {
        return;
    }
    
    int midX = (topLeft.x + botRight.x) / 2;
    int midY = (topLeft.y + botRight.y) / 2;
    
    topLeftTree = new Quad(topLeft, Point(midX, midY));
    topRightTree = new Quad(Point(midX, topLeft.y), Point(botRight.x, midY));
    botLeftTree = new Quad(Point(topLeft.x, midY), Point(midX, botRight.y));
    botRightTree = new Quad(Point(midX, midY), botRight);
    
    isLeaf = false;
}

void Quad::dynamicSubdivide(const cv::Mat &image, Point queryPoint) {
    if (!inBoundary(queryPoint)) return;
    
    if (isLeaf) {
        if ((botRight.x - topLeft.x) > resolution && (botRight.y - topLeft.y) > resolution) {
            subdivide();
            isLeaf = false;
        } else {
            loadRegion(image);
            return;
        }
    }
    
    if (topLeftTree && topLeftTree->inBoundary(queryPoint)) topLeftTree->dynamicSubdivide(image, queryPoint);
    else if (topRightTree && topRightTree->inBoundary(queryPoint)) topRightTree->dynamicSubdivide(image, queryPoint);
    else if (botLeftTree && botLeftTree->inBoundary(queryPoint)) botLeftTree->dynamicSubdivide(image, queryPoint);
    else if (botRightTree && botRightTree->inBoundary(queryPoint)) botRightTree->dynamicSubdivide(image, queryPoint);
}

bool Quad::inBoundary(Point p) {
    return (p.x >= topLeft.x && p.x < botRight.x && p.y >= topLeft.y && p.y < botRight.y);
}

Quad* Quad::getLeafContaining(Point p) {
    if (!inBoundary(p)) return nullptr;
    if (isLeaf) return this;
    
    if (topLeftTree && topLeftTree->inBoundary(p)) return topLeftTree->getLeafContaining(p);
    if (topRightTree && topRightTree->inBoundary(p)) return topRightTree->getLeafContaining(p);
    if (botLeftTree && botLeftTree->inBoundary(p)) return botLeftTree->getLeafContaining(p);
    if (botRightTree && botRightTree->inBoundary(p)) return botRightTree->getLeafContaining(p);
    
    return nullptr;
}

Quad* Quad::findSmallestCommonAncestor(Point p1, Point p2) {
    if (!inBoundary(p1) || !inBoundary(p2)) return nullptr;
    if (isLeaf) return this;
    
    Quad* child = nullptr;
    if (topLeftTree && topLeftTree->inBoundary(p1) && topLeftTree->inBoundary(p2)) child = topLeftTree;
    else if (topRightTree && topRightTree->inBoundary(p1) && topRightTree->inBoundary(p2)) child = topRightTree;
    else if (botLeftTree && botLeftTree->inBoundary(p1) && botLeftTree->inBoundary(p2)) child = botLeftTree;
    else if (botRightTree && botRightTree->inBoundary(p1) && botRightTree->inBoundary(p2)) child = botRightTree;
    
    return child ? child->findSmallestCommonAncestor(p1, p2) : this;
}

cv::Mat Quad::extractRegionContainingPoints(Point p1, Point p2, const cv::Mat &image) {
    Quad* ancestor = findSmallestCommonAncestor(p1, p2);
    if (!ancestor) return cv::Mat();
    
    ancestor->loadRegion(image);
    return ancestor->region;
}

void Quad::loadRegion(const cv::Mat &image) {
    if (region.empty()) {
        cv::Rect roi(topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);
        region = image(roi).clone();
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    cv::Mat image = cv::imread("carina_nebula.png");
    if (image.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Create the global quadtree covering the entire map
    Quad quadtree(Point(0, 0), Point(image.cols, image.rows));

    // Setup a FIFO queue for waypoints (global path)
    std::queue<Point> waypointQueue;
    // For demonstration, adding a series of waypoints along a path
    waypointQueue.push(Point(400, 400));
    waypointQueue.push(Point(450, 450));
    waypointQueue.push(Point(500, 500));
    waypointQueue.push(Point(550, 550));
    waypointQueue.push(Point(600, 600));
    waypointQueue.push(Point(650, 650));
    waypointQueue.push(Point(700, 700));

    // Cache to store the last N cached nodes (local sub-nodes)
    const int cacheSize = 3;
    std::deque<Quad*> nodeCache;

    while (!waypointQueue.empty()) {
        Point currentWaypoint = waypointQueue.front();
        waypointQueue.pop();

        // Dynamically subdivide for the current waypoint
        quadtree.dynamicSubdivide(image, currentWaypoint);
        // Retrieve the leaf node containing the current waypoint
        Quad* leaf = quadtree.getLeafContaining(currentWaypoint);
        if (leaf) {
            // Add to the cache
            nodeCache.push_back(leaf);
            // If cache exceeds size, remove the oldest cached node
            if (nodeCache.size() > cacheSize) {
                nodeCache.pop_front();
            }
            // For visualization: show the region corresponding to the current leaf node
            if (!leaf->region.empty()) {
                std::cout << "Displaying region for leaf node containing waypoint (" << currentWaypoint.x << ", " << currentWaypoint.y << ")" << std::endl;
                cv::imshow("Current Leaf Region", leaf->region);
                // Wait 500ms between waypoints to simulate movement along the path
                cv::waitKey(1000);
            }
        }
    }

    // Optionally, save the last cached node's region as an example
    if (!nodeCache.empty()) {
        Quad* lastCached = nodeCache.back();
        if (lastCached && !lastCached->region.empty()) {
            cv::imwrite("extracted_map/cached_leaf_region.png", lastCached->region);
        }
    }


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n"; 
    return 0;

}

// This is not working waypoints in queue. check the code and fix the issue.