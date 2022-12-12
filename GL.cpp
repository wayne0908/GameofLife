//
//  main.cpp
//  GL
//
//  Created by liweizhi on 2022/12/11.
//  Copyright © 2022 liweizhi. All rights reserved.
//

#include <iostream>
#include <fstream>  
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

void CreateGliderPattern(bool **States, int Row, int Col){
    for (int i = 0; i < Row; i++) {
        
        // Declare a memory block
        // of size n
        States[i] = new bool[Col];
    }
    // States[Row][Col] = {};
    
    States[0][1] = 1;
    States[1][2] = 1;
    for (int i = 0; i < 3; i++)
        States[2][i] = 1;
}

void UpdateStates(bool **States, bool *Buffer, int CurrentRow, int Row, int Col){
    int NewRow, NewCol, AliveNum=0;
    int Moves[8][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; // move to eight neighbors
    for (int j = 0; j < Col; j++){
        if (States[CurrentRow][j] == 1){// for alive cells
            for (int i = 0; i<8; i++){
                NewRow = CurrentRow + Moves[i][0]; NewCol =  j + Moves[i][1];
                if (NewRow >=0 && NewRow < Row && NewCol >=0 && NewCol < Col){
                    AliveNum+=States[NewRow][NewCol];
                }
            }
            if (AliveNum == 2 || AliveNum == 3){
                Buffer[j] = 1;
            }
            else
                Buffer[j] = 0;
        }
        else{// for dead cells
            for (int i = 0; i<8; i++){
                NewRow = CurrentRow + Moves[i][0]; NewCol =  j + Moves[i][1];
                if (NewRow >=0 && NewRow < Row && NewCol >=0 && NewCol < Col){
                    AliveNum+=States[NewRow][NewCol];
                }
            }
            if (AliveNum == 3){
                Buffer[j] = 1;
            }
            else
                Buffer[j] = 0;
        }
        AliveNum = 0;
    }
    
}

void AssignValues(bool **States, bool *Buffer, int CurrentRow, int Col){
    for (int i = 0; i < Col; i++){
        States[CurrentRow][i] = Buffer[i];
    }
}

void MoveCursorsToOriginal(int Row){
    for (int i = 0; i < Row ; i++){
        cout << "\x1b[A";
    }
}
void StartGame(bool **States, int Row, int Col){
    bool FirstBuffer[Row], SecondBuffer[Col];
    int  SelectBuffer = 0; // SelectBuffer = 0 indicates FirstBuffer selection and
    // SelectBuffer = 1 indicates SecondBuffer section
    
    for (int i = 0; i < Row; i++){
        if (SelectBuffer){
            UpdateStates(States, SecondBuffer, i, Row, Col);
            AssignValues(States, FirstBuffer, i - 1, Col);
            
        }
        else{
            UpdateStates(States, FirstBuffer, i, Row, Col);
            if (i != 0){
                AssignValues(States, SecondBuffer, i - 1, Col);
            }
            
        }
        SelectBuffer = !SelectBuffer;
    }
    if (SelectBuffer){
        AssignValues(States, FirstBuffer, Row-1, Col);
    }
    else{
        AssignValues(States, SecondBuffer, Row-1, Col);
    }
}


void SaveStatus(bool **States, int Row, int Col){
	string filename;
	cout << "Please input the name of the file to be saved:"<<endl; 
	cin >> filename;
	ofstream fout(filename + ".txt");
	if(fout.is_open()){
		for(int i = 0; i<Row; i++){
			for(int j = 0; j < Col;j++){
				fout << States[i][j]; 
			}
	      	fout<<'\n';
		}
	    cout << "Saved Success!" << endl;
	}
	else{
		cout << "File could not be opened." << endl;
	}
}

int LoadStatus(bool **States){
	int Row = 0, Col = 0;
	char Val; 

	cout << "Please input the name of the file to be loaded:"<<endl; 
	string filename; 
	cin >> filename;
	ifstream infile(filename + ".txt");	

	if (!infile.is_open()) return -1; 

	while (infile.get(Val))
	{
	    if(Val == '\n'){
	    	break;

	    }
	    Row+=1;
	}	

    infile.seekg (0, ios::beg);
    int i = 0;
	while (infile.get(Val))
	{
	    if(Val == '\n'){
	    	Col=0;
	    	i+=1;

	    }
	    else{
	    	if (Val == '1'){
	    		States[i][Col] = 1;
	    	}	
	    	else{
	    		States[i][Col] = 0;
	    	}	
	    	Col+=1;

	    }
	    
	}
	infile.close();
	return Row;
}


void PrintStates(bool **States, int Row, int Col){
    for(int i = 0; i < Row; i++){
        for(int j = 0; j < Col; j++){
            std::cout << States[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


int main(){
	/* Initialize the univese and start the game.*/
	char K; // Accept key input to decide whether load or create a new universe
	int Row; // size of the universe
    cout << "Press 'N' to create a new universe,\nPress 'L' to load an existing universe,\nor Press other keys to exit the game:"<<endl;
    cin >> K;
    if (K != 'N' and K != 'L'){
    	return 0;
    }
    else{
    	bool** States = new bool*[1];
	    if (K == 'N'){// create a new universe
	    	cout << "Game Start! Please enter row number (at least 3):"<<endl;
			cin >> Row;
			if (Row <3){
				cout << "Error: the row number is smaller than 3"<<endl;
				return 0;
	    	}
	    	// States = new bool*[Row];
	    	States = new bool*[100];
	    	CreateGliderPattern(States, Row, Row);
	    	PrintStates(States, Row, Row);
	    }
	    else if (K == 'L'){// Load an universe
	    	// bool** States = new bool*[100];
	    	// States[0] = new bool[1];
	    	States = new bool*[500];

		    for (int i = 0; i < 500; i++) {
		        
		        // Declare a memory block
		        // of size n
		        States[i] = new bool[500];
		    }
	    	Row = LoadStatus(States);
	    	if (Row == -1){
	    		cout<<"Incorrect filename.";
	    		return 0;
	    	}

	    	PrintStates(States, Row, Row);
	    	
	    }
	   	/* Update universe states.*/
	    char D = 'C'; // Accept key input to decide whether save, reset or continue
	    int Count = 0;
	    while(D != 'E' and D!= 'e'){
	        Count+=1;
	        StartGame(States, Row, Row);
	        // cout << "iteration:"<<Count<<endl;
	        MoveCursorsToOriginal(Row);
	        usleep(1000000);
	        PrintStates(States, Row, Row);
	        if(Count % 5 == 0){
	        	cout<<"Press key S to reset, E to save and exit, or press any key to continue\n"; 
	        	cin >> D;
		        if (D == 'E' or D == 'e'){
		        	SaveStatus(States, Row, Row);
		        }
		        	
		        else if (D == 'S'){
		        	CreateGliderPattern(States, Row, Row);
		        	PrintStates(States, Row, Row);
		        }
		        else{
		        	PrintStates(States, Row, Row);
		        }
	        }
	    }
	    cout << "Game End!"<<endl;
    }
	return 0;
}



