//Nicolas Stoian
//CS780 Image Processing
//Project 6 - Sobel Edge Detector

//This program needs 3 command line arguments
//argv[1] "input" for text file representing the binary image
//argv[2] "output1" to write SobelVHSum as an image with header for future processing
//argv[3] "output2" to write SobelDiagSum as an image with header for future processing

#include <fstream>
using namespace std;

class ImageProcessing{ friend class SobelEdge;

private:
    int** imgAry;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;

public:
    ImageProcessing();
    ImageProcessing(ifstream& inFile);
    ~ImageProcessing();
    void loadImage(ifstream& inFile);
    void mirrorFramed();
    static string mapInt2Char(int theInt);
    void prettyPrint(ofstream& outFile);
    void printImage(ofstream& outFile);
    void printMirroredImage(ofstream& outFile);

};

class SobelEdge{

private:
    int maskVertical[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
    int maskHorizontal[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
    int maskRightDiag[3][3] = {{0, 1, 2}, {-1, 0, 1}, {-2, -1, 0}};
    int maskLeftDiag[3][3] = {{2, 1, 0}, {1, 0, -1}, {0, -1, -2}};
    int numRows;
    int numCols;
    int** SobelVertical;
    int** SobelHorizontal;
    int** SobelRightDiag;
    int** SobelLeftDiag;
    int** SobelVHSum;
    int** SobelDiagSum;

public:
    SobelEdge();
    SobelEdge(ImageProcessing* image);
    ~SobelEdge();
    void printSobelVHSum(ofstream& outFile);
    void printSobelDiagSum(ofstream& outFile);
    void computeSobel(ImageProcessing* image);
    static int convolute(int neghborhood[3][3], int mask[3][3]);
    static int abs(int value);
};


int main(int argc, char* argv[]){
    ifstream inFile1;
    inFile1.open(argv[1]);
    ImageProcessing* image = new ImageProcessing(inFile1);
    inFile1.close();
    SobelEdge* sobel = new SobelEdge(image);
    sobel->computeSobel(image);
    ofstream outFile1;
    outFile1.open(argv[2]);
    sobel->printSobelVHSum(outFile1);
    outFile1.close();
    ofstream outFile2;
    outFile2.open(argv[3]);
    sobel->printSobelDiagSum(outFile2);
    outFile2.close();
}


ImageProcessing::ImageProcessing(): imgAry(NULL), numRows(0), numCols(0), minVal(0), maxVal(0){
}

ImageProcessing::ImageProcessing(ifstream& inFile){
    loadImage(inFile);
}

ImageProcessing::~ImageProcessing(){
    if(imgAry != NULL){
        for(int i = 0; i < numRows; i++){
            delete [] imgAry[i];
        }
    }
    delete [] imgAry;
}

void ImageProcessing::loadImage(ifstream& inFile){
    inFile >> numRows;
    inFile >> numCols;
    inFile >> minVal;
    inFile >> maxVal;
    imgAry = new int* [numRows + 2];
    for(int i = 0; i < numRows + 2; i++){
        imgAry[i] = new int [numCols + 2];
    }
    for(int row = 0; row < numRows + 2; row++){
        for(int col = 0; col < numCols + 2; col++){
            imgAry[row][col] = 0;
        }
    }
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            int value;
            inFile >> value;
            imgAry[row][col] = value;
        }
    }
    mirrorFramed();
}

void ImageProcessing::mirrorFramed(){

    for(int col = 0; col <= numCols + 1; col++){
        imgAry[0][col] = imgAry[1][col];
    }
    for(int row = 0; row <= numRows + 1; row++){
        imgAry[row][0] = imgAry[row][1];
    }
    for(int col = 0; col <= numCols + 1; col++){
        imgAry[numRows + 1][col] = imgAry[numRows][col];
    }
    for(int row = 0; row <= numRows + 1; row++){
        imgAry[row][numCols + 1] = imgAry[row][numCols];
    }
}

string ImageProcessing::mapInt2Char(int theInt){
    char toReturn [33];
    sprintf(toReturn, "%d", theInt);
    //itoa(theInt, toReturn, 10);
    return toReturn;
}

void ImageProcessing::prettyPrint(ofstream& outFile){
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(imgAry[row][col] <= 0){
                outFile << " " << " ";
            }
            else{
                outFile << mapInt2Char(imgAry[row][col]) << " ";
            }
        }
        outFile << endl;
    }
}

void ImageProcessing::printImage(ofstream& outFile){
    outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            outFile << mapInt2Char(imgAry[row][col]) << " ";
        }
        outFile << endl;
    }
}

void ImageProcessing::printMirroredImage(ofstream& outFile){
    outFile << numRows + 2 << " " << numCols + 2 << " " << minVal << " " << maxVal << endl;
    for(int row = 0; row < numRows + 2; row++){
        for(int col = 0; col < numCols + 2; col++){
            outFile << mapInt2Char(imgAry[row][col]) << " ";
        }
        outFile << endl;
    }
}

SobelEdge::SobelEdge(): numRows(0), numCols(0), SobelVertical(NULL), SobelHorizontal(NULL), SobelRightDiag(NULL), SobelLeftDiag(NULL), SobelVHSum(NULL), SobelDiagSum(NULL){
}

SobelEdge::SobelEdge(ImageProcessing* image){
    numRows = image->numRows;
    numCols = image->numCols;
    SobelVertical = new int* [numRows + 2];
    SobelHorizontal = new int* [numRows + 2];
    SobelRightDiag = new int* [numRows + 2];
    SobelLeftDiag = new int* [numRows + 2];
    SobelVHSum = new int* [numRows + 2];
    SobelDiagSum = new int* [numRows + 2];
    for(int i = 0; i < numRows + 2; i++){
        SobelVertical[i] = new int [numCols + 2];
        SobelHorizontal[i] = new int [numCols + 2];
        SobelRightDiag[i] = new int [numCols + 2];
        SobelLeftDiag[i] = new int [numCols + 2];
        SobelVHSum[i] = new int [numCols + 2];
        SobelDiagSum[i] = new int [numCols + 2];
    }
    for(int row = 0; row < numRows + 2; row++){
        for(int col = 0; col < numCols + 2; col++){
            SobelVertical[row][col] = 0;
            SobelHorizontal[row][col] = 0;
            SobelRightDiag[row][col] = 0;
            SobelLeftDiag[row][col] = 0;
            SobelVHSum[row][col] = 0;
            SobelDiagSum[row][col] = 0;
        }
    }
}

SobelEdge::~SobelEdge(){
    if(SobelVertical != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelVertical[i];
        }
    }
    delete [] SobelVertical;
    if(SobelHorizontal != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelHorizontal[i];
        }
    }
    delete [] SobelHorizontal;
    if(SobelRightDiag != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelRightDiag[i];
        }
    }
    delete [] SobelRightDiag;
    if(SobelLeftDiag != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelLeftDiag[i];
        }
    }
    delete [] SobelLeftDiag;
    if(SobelVHSum != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelVHSum[i];
        }
    }
    delete [] SobelVHSum;
    if(SobelDiagSum != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] SobelDiagSum[i];
        }
    }
    delete [] SobelDiagSum;
}

int SobelEdge::convolute(int neghborhood[3][3], int mask[3][3]){
    int result = 0;
    for(int row = 0; row < 3; row++){
        for(int col = 0; col < 3; col++){
            result += neghborhood[row][col] * mask[row][col];
        }
    }
    return result;
}

int SobelEdge::abs(int value){
    if(value < 0){
        return -value;
    }
    else{
        return value;
    }
}

void SobelEdge::printSobelVHSum(ofstream& outFile){
    int maxVal = 0;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(SobelVHSum[row][col] > maxVal){
                maxVal = SobelVHSum[row][col];
            }
        }
    }
    int minVal = maxVal;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(SobelVHSum[row][col] < minVal){
                minVal = SobelVHSum[row][col];
            }
        }
    }
    outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            outFile << ImageProcessing::mapInt2Char(SobelVHSum[row][col]) << " ";
        }
        outFile << endl;
    }
}

void SobelEdge::printSobelDiagSum(ofstream& outFile){
    int maxVal = 0;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(SobelVHSum[row][col] > maxVal){
                maxVal = SobelVHSum[row][col];
            }
        }
    }
    int minVal = maxVal;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(SobelVHSum[row][col] < minVal){
                minVal = SobelVHSum[row][col];
            }
        }
    }
    outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            outFile << ImageProcessing::mapInt2Char(SobelDiagSum[row][col]) << " ";
        }
        outFile << endl;
    }
}

void SobelEdge::computeSobel(ImageProcessing* image){
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            int neighborhood[3][3];
            for(int k = 0; k < 3; k++){
                for(int m = 0; m < 3; m++){
                    neighborhood[k][m] = image->imgAry[(row - 1) + k][(col - 1) + m];
                }
            }
            SobelVertical[row][col] = convolute(neighborhood, maskVertical);
            SobelHorizontal[row][col] = convolute(neighborhood, maskHorizontal);
            SobelRightDiag[row][col] = convolute(neighborhood, maskRightDiag);
            SobelLeftDiag[row][col] = convolute(neighborhood, maskLeftDiag);
            SobelVHSum[row][col] = abs(SobelVertical[row][col] + abs(SobelHorizontal[row][col]));
            SobelDiagSum[row][col] = abs(SobelRightDiag[row][col] + abs(SobelLeftDiag[row][col]));
        }
    }
}
