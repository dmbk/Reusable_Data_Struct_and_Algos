#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <iostream> 
#include <cmath> 
#include <vector>
#include <stack>
#include <utility>
#include <stdint.h>

using namespace std; 

//Point in the 2D plane
template <typename S>
struct Point 
{ 
    S x; 
    S y; 
    Point(S _x, S _y) 
    { 
        x = _x; 
        y = _y; 
    } 
    Point() 
    { 
        x = 0; 
        y = 0; 
    } 
};

//Tree node that contains the position, and data 
template <typename T, typename S>
struct Node 
{ 
    Point<S> point; 
    T* tag; 
    //The tag is an object provided by the user and should be deleted from users end
    Node(Point<S> _point, T* _tag) 
    { 
        point = _point; 
        tag = _tag; 
    } 
    Node() 
    { 
        tag = NULL; 
    }
}; 

/*****************************************************************
 * Class Name      : QuadBox 
 *  
 * Base Class      : None
 *  
 * Purpose         : The QuadTree structure is maintained using this class
 *  
 * Special Members : topLeft,topRight - the marks of the bounding box
 * leaf -  if the box is containing a leaf nodem this variable is set
 * top*Tree,bottom*Tree - The four child members that are pointing to QuadBoxes dividing the current QuadBox into equally sized 4 QuadBoxes 
 * templates       : T - The data object type stored in leaf nodes
 * S - Data type of cordinates
 *  
 ****************************************************************/
template <typename T, typename S>
class QuadBox 
{ 
    Point<S> topLeft; 
    Point<S> bottomRight; 
  
    Node<T,S> *leaf; 
  
    QuadBox *topLeftTree; 
    QuadBox *topRightTree; 
    QuadBox *bottomLeftTree; 
    QuadBox *bottomRightTree; 
    int leafCount; 
public: 
    QuadBox() 
    { 
        topLeft = Point<S>(0, 0); 
        bottomRight = Point<S>(0, 0); 
        leaf = NULL; 
        topLeftTree  = NULL; 
        topRightTree = NULL; 
        bottomLeftTree  = NULL; 
        bottomRightTree = NULL;
        leafCount = 0;
    } 
    QuadBox(Point<S> _topLeft, Point<S> _bottomRight) 
    { 
        leaf = NULL; 
        topLeftTree  = NULL; 
        topRightTree = NULL; 
        bottomLeftTree  = NULL; 
        bottomRightTree = NULL; 
        topLeft = _topLeft; 
        bottomRight = _bottomRight;
        leafCount = 0;
    } 
    ~QuadBox()
    {
        if(topLeftTree) delete topLeftTree;
        if(topRightTree) delete topRightTree;
        if(bottomLeftTree) delete bottomLeftTree;
        if(bottomRightTree) delete bottomRightTree;
        if(leaf) delete leaf;
        leaf = NULL; 
        topLeftTree  = NULL; 
        topRightTree = NULL; 
        bottomLeftTree  = NULL; 
        bottomRightTree = NULL; 
    }
    void insert(Node<T,S>*); 
    Node<T,S>* search(Point<S>, stack<QuadBox<T,S>*>& parentStack);
    bool isInQuadBox(Point<S>);
    QuadBox* getTopLeftTree(){return topLeftTree;}
    QuadBox* getTopRightTree(){return topRightTree;}
    QuadBox* getBottomLeftTree(){return bottomLeftTree;}
    QuadBox* getBottomRightTree(){return bottomRightTree;}
    Point<S> getTopLeft(){return topLeft;}
    Point<S> getBottomRight(){return bottomRight;}
}; 

/*****************************************************************
 * Class Name      : QuadTree 
 *  
 * Base Class      : None
 *  
 * Purpose         : The instantiable class for using the QuadTree from outside. 
 * This is a singleton clas.Contains all the functions for querying/inserting the quad tree
 *  
 * Special Members : rootBox - the root of the QuadTree, a QuadBox object
 * currentQueryPoint - This is set to the given query point when searching the tree for nearest nodes
 * p2pDistance - The distance function to be provided by the user. point to point distance
 *
 * templates       : T - The data object type stored in leaf nodes
 * S - Data type of cordinates
 *  
 ****************************************************************/
template <typename T, typename S>
class QuadTree
{
    public:
        static QuadTree<T,S>& getInstance(S lonTopLeft, S latTopLeft, S lonBottomRight, S latBottomRight,  double (*_p2pDistance)(Point<S>, Point<S>))
        {
            static QuadTree<T,S> instance(lonTopLeft, latTopLeft, lonBottomRight, latBottomRight, _p2pDistance); 
            return instance;
        }

        void insertNode(Point<S> point, T* tag);
        Node<T,S>* searchNode(Point<S> point){
            return rootBox->search(point);
        }
        void searchKNearestNodes(Point<S> point, vector<pair<Node<T,S>*,double> >& kNearestNodes, uint32_t k);

        bool isValidPoint(Point<S> point){return rootBox->isInQuadBox(point);}
        void clear(){
            if(rootBox)
                delete rootBox;
            rootBox = NULL;
        }

    private:
        Point<S> currentQueryPoint;
        double (*p2pDistance)(Point<S>, Point<S>);
        QuadBox<T,S>* rootBox;
        ~QuadTree(){
            if(rootBox)
                delete rootBox;
            rootBox = NULL;
        }
        QuadTree(){}
        QuadTree(S lonTopLeft, S latTopLeft, S lonBottomRight, S latBottomRight, double (*_p2pDistance)(Point<S>, Point<S>))
        {
            rootBox = new QuadBox<T,S>(Point<S>(lonTopLeft, latTopLeft), Point<S>(lonBottomRight, latBottomRight));
            p2pDistance = _p2pDistance;
        }
        QuadTree(const QuadTree<T,S>&) = delete;              
        QuadTree<T,S>& operator=(const QuadTree<T,S>&) = delete;
        void addToNearestNodesVector(vector<pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k, Node<T,S>* node, double distance);
        void searchSubTree(QuadBox<T,S>* subTreeBox, Point<S> point, vector< pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k);
        Point<S> closestPointToQueryPoint(QuadBox<T,S>* subTreeBox);
};

#include "QuadTree.tpp"   // importing the templated function definitions

#endif //QUAD TREE
