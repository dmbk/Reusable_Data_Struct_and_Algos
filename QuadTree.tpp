/*******************************************************************
   Function   : insert(Node<T,S> *node)
 
   Purpose    : Inserts a Node object into the QuadTree by traversing down the QuadBoxes. 
   Expands and creates new QuadBoxes by dividing the current QuadBox creating a hierarchical tree 
   It is expanded only if there is a need to add another node under the same subTree. 
   If any QuadBox contains a leaf, that QuadBox is a leaf node of the QuadTree. 
   If the QuadBox is divided the leaf node is withdrawn and added in the hierarchy below. 
 
   Called From: QuadTree::insertNode
 
   Parameters : node - the node to be inserted 
   Returns    : void 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>  
void QuadBox<T,S>::insert(Node<T,S> *node) 
{ 
    if (node == NULL) 
        return; 
  
    if (!isInQuadBox(node->point)) 
        return; 
   
    if(leaf && leaf->point.x == node->point.x && leaf->point.y == node->point.y){
        return; //will not be replacing the nodes with exact same values
    }

    if(0 == leafCount && !leaf)
    {
        leafCount++;
        leaf = node; 
        return; 
    }
    if ((topLeft.x + bottomRight.x) / 2 >= node->point.x) 
    { 
        if ((topLeft.y + bottomRight.y) / 2 <= node->point.y) 
        { //Top Left
            if (topLeftTree == NULL) 
                topLeftTree = new QuadBox<T,S>( 
                    Point<S>(topLeft.x, topLeft.y), 
                    Point<S>((topLeft.x + bottomRight.x) / 2, 
                        (topLeft.y + bottomRight.y) / 2)); 
            topLeftTree->insert(node); 
        } 
        else 
        { //Bottom Left
            if (bottomLeftTree == NULL) 
                bottomLeftTree = new QuadBox<T,S>( 
                    Point<S>(topLeft.x, 
                        (topLeft.y + bottomRight.y) / 2), 
                    Point<S>((topLeft.x + bottomRight.x) / 2, 
                        bottomRight.y)); 
            bottomLeftTree->insert(node); 
        } 
    } 
    else
    { 
        if ((topLeft.y + bottomRight.y) / 2 <= node->point.y) 
        { //Top Right
            if (topRightTree == NULL) 
                topRightTree = new QuadBox<T,S>( 
                    Point<S>((topLeft.x + bottomRight.x) / 2, 
                        topLeft.y), 
                    Point<S>(bottomRight.x, 
                        (topLeft.y + bottomRight.y) / 2)); 
            topRightTree->insert(node); 
        } 
        else
        { //Bottom Right
            if (bottomRightTree == NULL) 
                bottomRightTree = new QuadBox<T,S>( 
                    Point<S>((topLeft.x + bottomRight.x) / 2, 
                        (topLeft.y + bottomRight.y) / 2), 
                    Point<S>(bottomRight.x, bottomRight.y)); 
            bottomRightTree->insert(node); 
        } 
    } 

    if(1 == leafCount && leaf)
    {
        Node<T,S> *tmpLeaf = leaf;
        leaf = NULL;
        this->insert(tmpLeaf);
        return;
    }

    leafCount++;
} 
 

/*******************************************************************
   Function   : search(Point<S> point, stack<QuadBox<T,S>*>& parentStack)
 
   Purpose    : Searches a given point recuresively down the QuadTree hierarchy
   It creates a parentStack while traversing which could be later used for k nearest neighbour searching
 
   Called From: QuadTree::searchSubTree
 
   Parameters : point - the node being searched, parentStack - QuadBox parents from the root 
   Returns    : Node<T,S>
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
Node<T,S>* QuadBox<T,S>::search(Point<S> point, stack<QuadBox<T,S>*>& parentStack) 
{ 
    parentStack.push(this);
    if (!isInQuadBox(point)){ 
        return NULL; 

    }    
    if (NULL != leaf)
    { 
        return leaf; 
    }

    if ((topLeft.x + bottomRight.x) / 2 >= point.x) 
    { 
        if ((topLeft.y + bottomRight.y) / 2 <= point.y) 
        { 
            if (topLeftTree == NULL) 
                return NULL; 
            return topLeftTree->search(point, parentStack); 
        } 
  
        else
        { 
            if (bottomLeftTree == NULL) 
                return NULL; 
            return bottomLeftTree->search(point, parentStack); 
        } 
    } 
    else
    { 
        if ((topLeft.y + bottomRight.y) / 2 <= point.y) 
        { 
            if (topRightTree == NULL) 
                return NULL; 
            return topRightTree->search(point, parentStack); 
        } 
  
        else
        { 
            if (bottomRightTree == NULL) 
                return NULL; 
            return bottomRightTree->search(point, parentStack); 
        } 
    } 
} 

/*******************************************************************
   Function   : isInQuadBox(Point<S> point)
 
   Purpose    : Checks whether the given point is inside the QuadBox 
 
   Called From: QuadBox::insert, QuadBox::search
 
   Parameters : point - point being checked
   Returns    : bool
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
bool QuadBox<T,S>::isInQuadBox(Point<S> point) 
{ 
    return (point.x >= topLeft.x && 
        point.x <= bottomRight.x && 
        point.y <= topLeft.y && 
        point.y >= bottomRight.y); 
} 
 
/*******************************************************************
   Function   : insertNode(Point<S> point, T* tag)
 
   Purpose    : This is one of the APIs available for use for the users of the QuadTree class 
 
   Called From: outside for inserting nodes
 
   Parameters : point - cordinate point to insert, tag - Can be used to store any outside object 
   Returns    : void 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
void QuadTree<T,S>::insertNode(Point<S> point, T* tag) 
{ 
    Node<T,S>* node = new Node<T,S>(point, tag);
    rootBox->insert(node);
} 

/*******************************************************************
   Function   : addToNearestNodesVector(vector< pair<Node<T,S>*,double> >& kNearestNodes, uint32_t k, Node<T,S>* node, double distance)

 
   Purpose    : Inserts new nodes in the kNearestNodes vector.
   It always maintain a vector size upto 'k'
 
   Called From: QuadTree::insertNode
 
   Parameters : kNearestNodes - nearest node vector, 
   k - number of nearest neighbours, 
   node - node to added,
   distance - distance to the currentQueryPoint 
   Returns    : void 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
void QuadTree<T,S>::addToNearestNodesVector(vector< pair<Node<T,S>*,double> >& kNearestNodes, uint32_t k, Node<T,S>* node, double distance)
{
    int inserted = 0;

    for(typename vector< pair<Node<T,S>*,double> >::iterator it = kNearestNodes.begin(); it != kNearestNodes.end(); it++)        
    {
        if(distance < (*it).second){
            kNearestNodes.insert(it,pair<Node<T,S>*,double>(node,distance));
            inserted = 1;
            break;
        }
    }
    if(inserted && k < kNearestNodes.size()){
        kNearestNodes.pop_back();
    }
    if(!inserted && k > kNearestNodes.size()){
        kNearestNodes.push_back(pair<Node<T,S>*,double>(node,distance));
    }

}


/*******************************************************************
   Function   : searchKNearestNodes(Point<S> point, vector<pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k)
 
   Purpose    : This function is the API that is called from outside classes for k-nearest node calculation 
 
   Called From: outside classes 
   Parameters : point - point that needs to be searched for, kNearestNodes - nearest nodes vector, k - number of nodes
   Returns    : void 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
void QuadTree<T,S>::searchKNearestNodes(Point<S> point, vector<pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k)
{
    currentQueryPoint = point;

    searchSubTree(rootBox, point, kNearestNodes, k);
}

/*******************************************************************
   Function   : closestPointToQueryPoint(QuadBox<T,S>* subTreeBox)
 
   Purpose    : Checks if a QuadBox is worth traversing down to find a node that is closer to the currentQueryPoint than the ones found so far. This will stil do traversal if all k-nearest nodes are not filled. 
 
   Called From: QuadTree::searchSubTree
 
   Parameters : subTreeBox- QuadBox under check
   Returns    : Point<S> 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
Point<S> QuadTree<T,S>::closestPointToQueryPoint(QuadBox<T,S>* subTreeBox)
{
    Point<S> topLeft = subTreeBox->getTopLeft();
    Point<S> bottomRight = subTreeBox->getBottomRight();
    if(currentQueryPoint.x < topLeft.x && currentQueryPoint.y < topLeft.y && currentQueryPoint.y > bottomRight.y )
    {
        return Point<S>(topLeft.x, currentQueryPoint.y);
    }
    if(currentQueryPoint.x < topLeft.x && currentQueryPoint.y > topLeft.y){
        return Point<S>(topLeft.x, topLeft.y);
    }
    if(currentQueryPoint.x > topLeft.x && currentQueryPoint.x < bottomRight.x && currentQueryPoint.y > topLeft.y)
    {
        return Point<S>(currentQueryPoint.x, topLeft.y);
    }
    if(currentQueryPoint.x > bottomRight.x && currentQueryPoint.y > topLeft.y )
    {
        return Point<S>(bottomRight.x, topLeft.y);
    }
    if(currentQueryPoint.x > bottomRight.x && currentQueryPoint.y < topLeft.y && currentQueryPoint.y > bottomRight.y)
    {
        return Point<S>(bottomRight.x, currentQueryPoint.y);
    }
    if(currentQueryPoint.x > bottomRight.x && currentQueryPoint.y < bottomRight.y)
    {
        return Point<S>(bottomRight.x, bottomRight.y);
    }
    if(currentQueryPoint.x < bottomRight.x && currentQueryPoint.x > topLeft.x && currentQueryPoint.y < bottomRight.y)
    {
        return Point<S>(currentQueryPoint.x, bottomRight.y);
    }
    if(currentQueryPoint.x < topLeft.x && currentQueryPoint.y < bottomRight.y)
    {
        return Point<S>(topLeft.x, bottomRight.y);
    }

    return Point<S>(0,0); // waived
}


/*******************************************************************
   Function   : searchSubTree(QuadBox<T,S>* subTreeBox, Point<S> point, vector< pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k)
 
   Purpose    : Searches down the hierarchy of QuadBoxes to find the given point and the k-nearest neighbours. Searches the possible sibling nodes to check if any new nearest neighbour exists
 
   Called From: QuadTree::searchNode
 
   Parameters : subTreeBox - current root of the subtree, point - target point to traverse towards, kNearestNodes - nearest nodes vector, k -  number of nearest nodes 
   Returns    : void 
   Globals    : None
 
*******************************************************************/
template <typename T, typename S>
void QuadTree<T,S>::searchSubTree(QuadBox<T,S>* subTreeBox, Point<S> point, vector< pair<Node<T,S>*, double> >& kNearestNodes, uint32_t k)
{
    stack<QuadBox<T,S>*> parentStack;
    Node<T,S>* leafNode = subTreeBox->search(point, parentStack);
    if(leafNode)
    {
       double distance = p2pDistance(currentQueryPoint, leafNode->point);

       addToNearestNodesVector(kNearestNodes, k, leafNode, distance);
    }
    double currentMaxDistance = 0;
    QuadBox<T,S>* traversedBox = parentStack.top();
    parentStack.pop();
    while(!parentStack.empty()) 
    {
        Point<S> closestPointInBox;
        QuadBox<T,S>* yetToTraverseBox = parentStack.top();
        parentStack.pop();
        if(!kNearestNodes.empty())
            currentMaxDistance = kNearestNodes.back().second;

        QuadBox<T,S>* topLeftTree = yetToTraverseBox->getTopLeftTree();
        QuadBox<T,S>* topRightTree = yetToTraverseBox->getTopRightTree();
        QuadBox<T,S>* bottomLeftTree = yetToTraverseBox->getBottomLeftTree();
        QuadBox<T,S>* bottomRightTree = yetToTraverseBox->getBottomRightTree();

        if(topLeftTree && traversedBox != topLeftTree)
        {
            closestPointInBox = closestPointToQueryPoint(topLeftTree);
            if(kNearestNodes.size() < k || currentMaxDistance > p2pDistance(currentQueryPoint, closestPointInBox))
            {
                searchSubTree(topLeftTree, closestPointInBox, kNearestNodes, k);    
            }
        }
        if(topRightTree && traversedBox != topRightTree)
        {
            closestPointInBox = closestPointToQueryPoint(topRightTree);
            if(kNearestNodes.size() < k || currentMaxDistance > p2pDistance(currentQueryPoint, closestPointInBox))
            {
                searchSubTree(topRightTree, closestPointInBox, kNearestNodes, k);    
            }        
        }
        if(bottomLeftTree && traversedBox !=  bottomLeftTree)
        {
            closestPointInBox = closestPointToQueryPoint(bottomLeftTree);
            if(kNearestNodes.size() < k || currentMaxDistance > p2pDistance(currentQueryPoint, closestPointInBox))
            {
                searchSubTree(bottomLeftTree, closestPointInBox, kNearestNodes, k);    
            }        
        }
        if(bottomRightTree && traversedBox != bottomRightTree)
        {
            closestPointInBox = closestPointToQueryPoint(bottomRightTree);
            if(kNearestNodes.size() < k || currentMaxDistance > p2pDistance(currentQueryPoint, closestPointInBox))
            {
                searchSubTree(bottomRightTree, closestPointInBox, kNearestNodes, k);    
            }        
        }
    }
   
}




