
//四叉树的计算举例，考虑在地形上的应用


#include <iostream.h>
#include <math.h>
#include <stdlib.h>
#include <conio.h>

float 	     *fVerts;  //顶点数据
unsigned int *uiList;  //面的数据

#define LEAF_TYPE 0
#define NODE_TYPE 1

typedef struct vertex
{
	float x,y,z;
}VERTEX;

//可以考虑将uiBranches[4]充分利用，对非叶子节点作为分支的索引号，对叶子节点作为面的数据索引
//可以考虑将uiBranches[4]和面的数据合并使用指针来实现
typedef struct node
{
	int          bType;  //类型：节点、叶子
	VERTEX	     vBoundingCoordinates[4]; //4顶点索引
	unsigned int uiBranches[4];           //4个分支的索引号，4叉，对叶子节点无用
	unsigned int uiID;               //id号
	unsigned int uiParentID;         //父id号

	unsigned int uiVertexStrip1[8];       //32个面的数据索引,对叶子节点有用
	unsigned int uiVertexStrip2[8];
	unsigned int uiVertexStrip3[8];
	unsigned int uiVertexStrip4[8];
}NODE;

NODE *pNodeList;

void CreateNode(unsigned int Bounding[4],unsigned int ParentID,unsigned int NodeID);
void CreateGrid(unsigned int w,unsigned int h);
void ReportNode(NODE *pNode);
unsigned int CalcNodeNum(unsigned int max,unsigned int min);


void main()
{
	float GridWidth;   //Specified in squares/tris, not vertices
	float GridHeight;

	cout<<"Enter Grid Width: ";
	cin>>GridWidth;
	cout<<"\nEnter Grid Height: ";
	cin>>GridHeight;

	//建立网格顶点和面
	CreateGrid(GridWidth+1,GridHeight+1);

	//计算4叉树，叶的宽度为4，并分配空间
	unsigned int Temp =(GridWidth/4)*(GridWidth/4); //总的叶子数（包括根）
	unsigned int Temp2 = CalcNodeNum(Temp,4); //计算4叉树节点数
	pNodeList = (NODE*)malloc(sizeof(NODE)*Temp2);

	//绑定根节点坐标
	unsigned int uiBoundingCoordinates[] = {0,GridWidth,(GridHeight*(GridWidth+1)),((GridHeight)*(GridWidth+1))+GridWidth};

	CreateNode(uiBoundingCoordinates,0,0);

	ReportNode(pNodeList);
		
	cout<<"\nSize of tree: "<<sizeof(*pNodeList)*Temp2<<" bytes";

	free(pNodeList);
	free(fVerts);
	free(uiList);

	getch();
	return;
}

/*		uiBoundingCoordinates[0]		uiBoundingCoordinates[1]
		*--------------------------------------------------------------*
		-							       -
		-							       -
		-							       -
		-							       -
		-							       -
		-							       -
		-							       -
		-							       -
		-							       -
		*--------------------------------------------------------------*
		uiBoundingCoordinates[2]		uiBoundingCoordinates[3]

*/
	

void CreateNode(unsigned int Bounding[4],unsigned int ParentID,unsigned int NodeID)
{
	static unsigned int TotalTreeID = 0;
	
	unsigned int uiNodeType;
	unsigned int uiWidth,uiHeight;

	uiWidth = fVerts[(Bounding[1]*3)] - fVerts[(Bounding[0]*3)]; 
	uiHeight = fVerts[(Bounding[2]*3)+2] - fVerts[(Bounding[0]*3)+2]; 
	
	if(0.5*uiWidth==2)
	{uiNodeType = LEAF_TYPE;}
	else
	{uiNodeType = NODE_TYPE;}

	NODE *pNode = &pNodeList[NodeID];
	
	pNode->uiID = NodeID;
	pNode->uiParentID = ParentID;
	
	pNode->vBoundingCoordinates[0].x = fVerts[(Bounding[0]*3)];
	pNode->vBoundingCoordinates[0].y = fVerts[(Bounding[0]*3)+1];
	pNode->vBoundingCoordinates[0].z = fVerts[(Bounding[0]*3)+2];

	pNode->vBoundingCoordinates[1].x = fVerts[(Bounding[1]*3)];
	pNode->vBoundingCoordinates[1].y = fVerts[(Bounding[1]*3)+1];
	pNode->vBoundingCoordinates[1].z = fVerts[(Bounding[1]*3)+2];

	pNode->vBoundingCoordinates[2].x = fVerts[(Bounding[2]*3)];
	pNode->vBoundingCoordinates[2].y = fVerts[(Bounding[2]*3)+1];
	pNode->vBoundingCoordinates[2].z = fVerts[(Bounding[2]*3)+2];

	pNode->vBoundingCoordinates[3].x = fVerts[(Bounding[3]*3)];
	pNode->vBoundingCoordinates[3].y = fVerts[(Bounding[3]*3)+1];
	pNode->vBoundingCoordinates[3].z = fVerts[(Bounding[3]*3)+2];
	
	pNode->bType = uiNodeType;


	if(uiNodeType == LEAF_TYPE)
	{return;}
	else
	{	
		
	
		unsigned int BoundingBox[4];
		TotalTreeID++;	
		pNode->uiBranches[0] = TotalTreeID;
		
		BoundingBox[0] = Bounding[0]; //Top-Left i.e. b[0]
		BoundingBox[1] = Bounding[0]+((Bounding[1]-Bounding[0])/2); //Between b[0] and b[1]
		BoundingBox[2] = Bounding[0]+((Bounding[2]-Bounding[0])/2); //[between b[0] and b[2]
		BoundingBox[3] = Bounding[0]+((Bounding[2]-Bounding[0])/2)+((Bounding[1]-Bounding[0])/2); //middle of node
	

		CreateNode(BoundingBox,NodeID,TotalTreeID);

//************************************************************************************************************

		TotalTreeID++;	
		pNode->uiBranches[1] = TotalTreeID;
		
		BoundingBox[0] = Bounding[0]+((Bounding[1]-Bounding[0])/2); // Between b[0] and b[1]
		BoundingBox[1] = Bounding[1]; //Top-Right i.e.  b[1]
		BoundingBox[2] = Bounding[0]+((Bounding[2]-Bounding[0])/2)+((Bounding[1]-Bounding[0])/2); //middle of node
		BoundingBox[3] = Bounding[0]+((Bounding[2]-Bounding[0])/2)+((Bounding[1]-Bounding[0])); //between b[1] & b[3]

		CreateNode(BoundingBox,NodeID,TotalTreeID);

//************************************************************************************************************

		TotalTreeID++;	
		pNode->uiBranches[2] = TotalTreeID;
		
		BoundingBox[0] = Bounding[0]+((Bounding[2]-Bounding[0])/2); //between b[0] and b[2]
		BoundingBox[1] = Bounding[0]+((Bounding[2]-Bounding[0])/2)+((Bounding[1]-Bounding[0])/2); //middle of node
		BoundingBox[2] = Bounding[2]; //Bottom-Left i.e. b[2]
		BoundingBox[3] = Bounding[2]+((Bounding[3]-Bounding[2])/2); //between b[2] and b[3]

		CreateNode(BoundingBox,NodeID,TotalTreeID);

//************************************************************************************************************

		TotalTreeID++;	
		pNode->uiBranches[3] = TotalTreeID;
		
		BoundingBox[0] = Bounding[0]+((Bounding[2]-Bounding[0])/2)+((Bounding[1]-Bounding[0])/2); //middle of node
		BoundingBox[1] = Bounding[0]+((Bounding[2]-Bounding[0])/2) + uiWidth; //between b[1] and b[3]
		BoundingBox[2] = Bounding[2]+((Bounding[3]-Bounding[2])/2); //between b[2] and b[3]
		BoundingBox[3] = Bounding[3]; //Bottom-Right i.e. b[3]

		CreateNode(BoundingBox,NodeID,TotalTreeID);
	
	}
		

	return;
}



void ReportNode(NODE *pNode)
{
	cout<<"Root Node ID: "<<pNode[0].uiID;
	cout<<"\nRoots Bounding Volume:\n";
	cout<<pNode[0].vBoundingCoordinates[0].x<<","<<pNode[0].vBoundingCoordinates[0].y;
	cout<<","<<pNode[0].vBoundingCoordinates[0].z<<"--------->";
	cout<<pNode[0].vBoundingCoordinates[1].x<<","<<pNode[0].vBoundingCoordinates[1].y;
	cout<<","<<pNode[0].vBoundingCoordinates[1].z<<"\n";
	cout<<pNode[0].vBoundingCoordinates[2].x<<","<<pNode[0].vBoundingCoordinates[2].y;
	cout<<","<<pNode[0].vBoundingCoordinates[2].z<<"--------->";
	cout<<pNode[0].vBoundingCoordinates[3].x<<","<<pNode[0].vBoundingCoordinates[3].y;
	cout<<","<<pNode[0].vBoundingCoordinates[3].z<<"\n";
	
	cout<<"Roots Children:\n\n";
	int child = pNode[0].uiBranches[0];
	cout<<"\n\nChild 0: "<<child;
	cout<<"\nBounding Volume:\n";
	cout<<pNode[child].vBoundingCoordinates[0].x<<","<<pNode[child].vBoundingCoordinates[0].y;
	cout<<","<<pNode[child].vBoundingCoordinates[0].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[1].x<<","<<pNode[child].vBoundingCoordinates[1].y;
	cout<<","<<pNode[child].vBoundingCoordinates[1].z<<"\n";
	
	cout<<pNode[child].vBoundingCoordinates[2].x<<","<<pNode[child].vBoundingCoordinates[2].y;
	cout<<","<<pNode[child].vBoundingCoordinates[2].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[3].x<<","<<pNode[child].vBoundingCoordinates[3].y;
	cout<<","<<pNode[child].vBoundingCoordinates[3].z<<"\n";
	
	
	child = pNode[0].uiBranches[1];
	cout<<"\n\nChild 1 : "<<child;
	cout<<"\nBounding Volume:\n";
	cout<<pNode[child].vBoundingCoordinates[0].x<<","<<pNode[child].vBoundingCoordinates[0].y;
	cout<<","<<pNode[child].vBoundingCoordinates[0].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[1].x<<","<<pNode[child].vBoundingCoordinates[1].y;
	cout<<","<<pNode[child].vBoundingCoordinates[1].z<<"\n";
	
	cout<<pNode[child].vBoundingCoordinates[2].x<<","<<pNode[child].vBoundingCoordinates[2].y;
	cout<<","<<pNode[child].vBoundingCoordinates[2].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[3].x<<","<<pNode[child].vBoundingCoordinates[3].y;
	cout<<","<<pNode[child].vBoundingCoordinates[3].z<<"\n";

	child = pNode[0].uiBranches[2];
	cout<<"\n\nChild 2: "<<child;
	cout<<"\nBounding Volume:\n";
	cout<<pNode[child].vBoundingCoordinates[0].x<<","<<pNode[child].vBoundingCoordinates[0].y;
	cout<<","<<pNode[child].vBoundingCoordinates[0].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[1].x<<","<<pNode[child].vBoundingCoordinates[1].y;
	cout<<","<<pNode[child].vBoundingCoordinates[1].z<<"\n";
	
	cout<<pNode[child].vBoundingCoordinates[2].x<<","<<pNode[child].vBoundingCoordinates[2].y;
	cout<<","<<pNode[child].vBoundingCoordinates[2].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[3].x<<","<<pNode[child].vBoundingCoordinates[3].y;
	cout<<","<<pNode[child].vBoundingCoordinates[3].z<<"\n";
	
	child = pNode[0].uiBranches[3];
	cout<<"\n\nChild 3: "<<child;
	cout<<"\nBounding Volume:\n";
	cout<<pNode[child].vBoundingCoordinates[0].x<<","<<pNode[child].vBoundingCoordinates[0].y;
	cout<<","<<pNode[child].vBoundingCoordinates[0].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[1].x<<","<<pNode[child].vBoundingCoordinates[1].y;
	cout<<","<<pNode[child].vBoundingCoordinates[1].z<<"\n";
	
	cout<<pNode[child].vBoundingCoordinates[2].x<<","<<pNode[child].vBoundingCoordinates[2].y;
	cout<<","<<pNode[child].vBoundingCoordinates[2].z<<"--------->";
	cout<<pNode[child].vBoundingCoordinates[3].x<<","<<pNode[child].vBoundingCoordinates[3].y;
	cout<<","<<pNode[child].vBoundingCoordinates[3].z<<"\n";
}



void CreateGrid(unsigned int w,unsigned int h)
{
	unsigned int total=0;
	unsigned int width = w;
	unsigned int height = h;

	fVerts = (float*)malloc((sizeof(float))*width*height*3); /* Allocate space for 20 										    vertices with an x,y and z*/
	//顶点坐标初始化
	for(unsigned int ctr=0;ctr<height;ctr++)
	{
		for(unsigned int ctr2=0;ctr2<width;ctr2++)
		{				// Vertex number is total
			fVerts[total] =  ctr2;	// Vertex X component is ctr2
			fVerts[total+1] = 0;	// Vertex Y component is height field (0)
			fVerts[total+2] =  ctr;  // Vertex Z component is ctr
			total+=3;		// Increase total to be next vertex number
		}
	}	

	total=0;
	uiList = (unsigned int*)malloc((sizeof(unsigned int))*(width-1)*(height-1)*6);
	//三角形面，一个块有两个三角形面（6个顶点）
	//面中的顶点是顶点表中的索引号
	for(ctr=0;ctr<(height-1);ctr++)
	{
		for(unsigned int ctr2=0;ctr2<(width-1);ctr2++)
		{				       //Triangles are built in twos, hence 
		   				       //total+=6 to accomodate two sets of verts
			uiList[total] = (ctr*width)+ctr2;	  //Triangle A's 1st vertex
			uiList[total+1] = (ctr*width)+ctr2+width;
			uiList[total+2] = (ctr*width)+ctr2+1;
			uiList[total+3] = (ctr*width)+ctr2+width;   //Triangle B's first vertex
			uiList[total+4] = (ctr*width)+ctr2+width+1;
			uiList[total+5] = (ctr*width)+ctr2+1;
			total+=6;  //Increase total by six, because we're using two tri's a loop
		}
	}
}

//计算4叉树节点数
unsigned int CalcNodeNum(unsigned int max,unsigned int min)
{
	
	int ctr=0;
	int var = 0;

	while(var==0)
	{
		ctr+=max;
		max=max/min;
		
		if(max==1)
		{var=1;}
	}
	ctr++;
	
	return ctr;
}


