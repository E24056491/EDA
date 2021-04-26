#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<cmath>
#include <queue>

using namespace std;
static int counter = 0;
class Gate{
public:
	friend class Tree;
	Gate(string _name, string _out, string _in1, string _in2):name(_name),wire_out(_out),wire_in1(_in1),wire_in2(_in2),input1(0),input2(0),mincost(-1),bestPtn(-1){}
	string Get_wire_out(void){
		return wire_out;
	}
	int Getmincost(void){
        return mincost;
	}
	void Setmincost(int _c){
        mincost = _c;
	}

	void SetbestPtn(int p){
        bestPtn = p;
	}
	int GetbestPtn(void){
        return bestPtn;
	}

	Gate* Getinput(int _input){
		return (_input == 1)? input1: input2;
	}
	void Setinput(Gate* _input, int i){
		(i == 1) ? input1 = _input : input2 = _input;
	}
	int GetinputCounts(void){
		return ((!(input1 || input2)) ? 0 : (((!input1) != (!input2)) ? 1 : 2));
	}
	virtual string GetGateName(void) = 0;

protected:
	const string name, wire_in1, wire_in2, wire_out;
	int mincost, bestPtn;

	Gate *input1, *input2;
};
class Nand: public Gate{		//Nand inherits Gate
public:
    Nand(string _name, string _out, string _in1, string _in2):Gate(_name, _out, _in1, _in2){}
    string GetGateName(void) override {
		return "Nand";
	}
};
class Not: public Gate{			//Not inherits Gate, but has only one input
public:
    Not(string _name, string _out, string _in1):Gate(_name, _out, _in1, ""){}                  // Not has only one input, so set input2 to null
	string GetGateName(void) override {
		return "Not";
	}
};
class Tree{
public:
	Tree(Gate* _root){
		root = _root;
		height = Countheight(_root);
	}
	Gate* Getroot(void){
		return root;
	}

	string lvlStr(Gate *root, int lvl, string indent) {
		if (lvl == 1){		//level = 1表示到達要print的層
			if (root == 0){		// NULL或NULL的child要補0

			return indent + "0" + indent;
			}
		stringstream out;	//return name
        out << root->GetGateName();


		return indent + out.str() + indent;
		}
		  else if (lvl > 1){
		    string leftStr = lvlStr(root ? root->input1 : 0, lvl-1, indent);   // 當parent為NULL時，令他的child pointer為0
		    string rightStr = lvlStr(root ? root->input2 : 0, lvl-1, indent);

		    return leftStr + " " + rightStr; //將組起來的string回傳
		  }

		}

	void DisplayTree (void){
		Gate *current = root;
		for (int i = 1; i<=Getheight(); i++) {  //從level = 1，itorate 到 height
		    string indent="";				  //如果height大於6，只輸出8行
		    for (int j = 0; j < pow(2, Getheight()-i)-1; j++){  //第一層為2^height-1個空格
		      indent+=" ";      											//下一層為2^(height-1)-1個空格
		    }
		    string lvlNodes = lvlStr(current, i, indent);          //每一層的srting
		    cout << lvlNodes << endl;
		}
	}
	int Countheight(Gate* node){
		if (!node){
		    return 0;
		}
		return 1 + max(Countheight(node->input1), Countheight(node->input2));
	}
	void Setheight(void){
		height = Countheight(root);
	}
	int Getheight(void){
		return height;
	}
/*將flag2移除*/
protected:
	Gate* root;
	int height;

};
class Pattern_tree:public Tree{
public:
	Pattern_tree(Gate* _root, int _c):Tree(_root),cost(_c){}
	int Getcost(void){
        return cost;
	}
private:
	const int cost;
};

int mincost(Gate*,const vector<Pattern_tree*>&);
bool ismatched(Gate*, Gate*, vector<Gate*>&);

int main(int argc, char *argv[]){
	string gate, gate_name, wire_out, wire_in1, wire_in2;
	vector<Gate*> gates;
	Gate* root = 0;
	Tree *SubTree;
	ifstream circuit_file(argv[2]);
	cout << argv[2] << endl;

//    cout << "There are : " << argc << " arguments" << endl;
//
//    for (int i = 0; i != argc; ++i)
//    {
//        cout << argv[i] << std::endl;
//    }

    if(!circuit_file){
        cerr << "Failed opening" << endl;
        exit(1);
    }

    while(!circuit_file.eof()){
        circuit_file >> gate;
        Gate* curr_gate = 0;                    // ptr is used to point to previous bulit gates.
        if(gate == "NAND"){
            circuit_file>>gate_name>>wire_out>>wire_in1>>wire_in2;

            Nand* this_gate = new Nand(gate_name, wire_out, wire_in1, wire_in2);  // create a new gate(Nand)
            gates.push_back(this_gate);              							// 將新建立的的邏輯閘加入清單
            for(int i = 0;i < gates.size()-1; i++){

                curr_gate = gates.at(i);
                if(curr_gate->Get_wire_out() == wire_in1){
                    this_gate->Setinput(curr_gate, 1);
                }
                else if(curr_gate->Get_wire_out() == wire_in2){
                    this_gate->Setinput(curr_gate, 2);
                }
            }
        }
        else if(gate == "NOT"){
            circuit_file>>gate_name>>wire_out>>wire_in1;

            Not* this_gate = new Not(gate_name, wire_out, wire_in1);
            gates.push_back(this_gate);
            for(int i = 0; i < gates.size()-1; i++){
                curr_gate = gates.at(i);
                if(curr_gate->Get_wire_out() == wire_in1){
                    this_gate->Setinput(curr_gate, 1);

                }
            }
        }
        gate = "";
    }

    SubTree = new Tree(gates.back());
	
	cout<<"\n";
    SubTree->DisplayTree();
    
	circuit_file.close();
    gate = "";

/*********************complete Tree-ifying subject graph *****************************/
	circuit_file.close();
	circuit_file.open(argv[1]);
	if(!circuit_file){
		cerr << "\nFailed opening Pattern_Trees.txt";
		exit(1);
	}
	vector<Gate*> pat_gates[7];			//二維vector用來儲存pattern_tree，
	string pattern_name;
	int pat_order, pat_cost;
	vector<Pattern_tree*> PatTrees;

	while(!circuit_file.eof()){

		circuit_file>>pattern_name;
		pat_order = pattern_name.back() - '1';		//pat_order代表第幾個pattern tree


		while(gate != "cost"){		//如果gate讀到'cost'代表一個module結束
			circuit_file>>gate;
			Gate* curr_gate = 0;

			if(gate == "NAND"){
				circuit_file>>gate_name>>wire_out>>wire_in1>>wire_in2;


				Nand* this_gate = new Nand(gate_name, wire_out, wire_in1, wire_in2);  // create a new gate(Nand)
				pat_gates[pat_order].push_back(this_gate);              							// 將新建立的的邏輯閘加入清單

				for(int i = 0;i < pat_gates[pat_order].size()-1; i++){
					curr_gate = pat_gates[pat_order].at(i);
					if(curr_gate->Get_wire_out() == wire_in1){
						this_gate->Setinput(curr_gate, 1);
					}
					else if(curr_gate->Get_wire_out() == wire_in2){
						this_gate->Setinput(curr_gate, 2);
					}
				}
			}

			else if(gate == "NOT"){
				circuit_file>>gate_name>>wire_out>>wire_in1;

				Not* this_gate = new Not(gate_name, wire_out, wire_in1);
				pat_gates[pat_order].push_back(this_gate);

				for(int i = 0; i < pat_gates[pat_order].size()-1; i++){
					curr_gate = pat_gates[pat_order].at(i);
					if(curr_gate->Get_wire_out() == wire_in1){
						this_gate->Setinput(curr_gate, 1);
					}
				}
			}
		}
		circuit_file >> pat_cost;

		PatTrees.push_back(new Pattern_tree(pat_gates[pat_order].back(), pat_cost));			//用pat_gate的最後一個Gate當作root來建立樹
		gate = "";
	}
	//*******************************-******************

	cout << "minimum cost is " << mincost(SubTree->Getroot(), PatTrees);
}
int mincost(Gate* node,const vector<Pattern_tree*> &PatTrees){
    vector<Gate*> leaves;
    int cost = 99999;
    int flag;

    for(int i = 0;i < PatTrees.size(); i++){
        leaves.clear();
        flag = ismatched(node, PatTrees.at(i)->Getroot(), leaves);
        int newcost;
        if(flag){
            leaves.erase(remove(leaves.begin(), leaves.end(), nullptr), leaves.end());
            newcost = PatTrees.at(i)->Getcost();
        }
        else{
            leaves.clear();
            newcost = 9999999;     //dummy value to prevent newcost being smaller than cost
        }

        for(int j = 0; j < leaves.size(); j++){
            if(leaves.at(j)->Getmincost() != -1){       // if the node is visited before, get the previously-stored cost value
                newcost += leaves.at(j)->Getmincost();
            }
            else{
                newcost += mincost(leaves.at(j), PatTrees);
            }


        }

        if(newcost < cost){
            cost = newcost;
            node->SetbestPtn(i+1);
        }
    }
    node->Setmincost(cost);     //update the cost value

    return cost;
}

bool ismatched(Gate *root1, Gate *root2, vector<Gate*> &leaves){

	Gate* s_input1 = root1->Getinput(1);
	Gate* s_input2 = root1->Getinput(2);
	Gate* p_input1 = root2->Getinput(1);
	Gate* p_input2 = root2->Getinput(2);
	if(root1->GetGateName() == root2->GetGateName()){	// compare two subtree root first, if true ,there are three scenarios
		if(!(p_input1 || p_input2)){		//  reach pattern tree leave nodes

			leaves.push_back(s_input1);
			leaves.push_back(s_input2);
			return true;
		}
		else if(root1->GetinputCounts() < root2->GetinputCounts()){  //if there are no nodes in subject tree left to match pattern nodes
			return false;
		}
		else if(root1->GetinputCounts() >= root2->GetinputCounts()){ //can be further divided into three scenarios

		    if(root1->GetinputCounts() == 1 && root2->GetinputCounts() == 1){
				if(!s_input1){
					if(!p_input1){
						return ismatched(s_input2, p_input2, leaves);
					}
					else{
						return ismatched(s_input2, p_input1, leaves);
					}
				}
				else{
					if(!p_input1){
						return ismatched(s_input1, p_input2, leaves);
					}
					else{
						return ismatched(s_input1, p_input1, leaves);
					}
				}

			}
			else if(root1->GetinputCounts() == 2 && root2->GetinputCounts() == 1){
				int flag1, /*flag2, */temp_size;    /*將flag2移除*/


				if(!p_input1){  	// pattern tree input1 is null
					flag1 = ismatched(s_input1, p_input2, leaves);
				}
				else{				// pattern tree input2 is null
					flag1 = ismatched(s_input1, p_input1, leaves);
				}
//				check how pattern tree match in both inputs of the subject tree
				if(flag1 /*&& flag2*/){    // both inputs match patter graph
						leaves.push_back(s_input2);
						return true;
				}
//				else if(flag1 != flag2){	//only one match
//					flag1 ? leaves.push_back(s_input2) : leaves.push_back(s_input1);    /*將flag2移除*/
//					return true;
//				}
				else{	//does not match
					return false;
				}
			}
			else if(root1->GetinputCounts() == 2 && root2->GetinputCounts() ==2){
				if(s_input1->GetGateName() == s_input2->GetGateName() && s_input2->GetGateName() == p_input1->GetGateName() && p_input1->GetGateName() == p_input2->GetGateName()){
					return ismatched(s_input1, p_input1, leaves) && ismatched(s_input2, p_input2, leaves);
				}
				else{
					return (ismatched(s_input1, p_input1, leaves) && ismatched(s_input2, p_input2, leaves)) || (ismatched(s_input1, p_input2, leaves) && ismatched(s_input2, p_input1, leaves));
				}
			}
			else{
                cout << "this shouldnt happen";
                exit(2);
			}
		}
		else{
            cout << "this shouldnt happen";
            exit(1);
		}
	}
	else{
		return false;
	}
}

