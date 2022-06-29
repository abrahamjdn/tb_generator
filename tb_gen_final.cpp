///////////////////////////////////////////////////////////////////////////////////////
//////////////////// TEST BENCH GENERATOR /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <utility>

using namespace std;


const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}
string get_name_module(ifstream &file){
    file.seekg(0,ios::beg);
    string linea;
    if(file.fail()){
        cout << "Error";
        std::exit(EXIT_FAILURE);
    }
    while(!file.eof()){
        getline(file,linea);
        if(linea.find("module") != string::npos){
            size_t pos0=linea.find("module");

            if(linea.find('(') != string::npos){
                size_t pos1=linea.find('(');
                linea=linea.substr(pos0,pos1-pos0);
                linea=linea.substr(6);
            }
            else{
                linea=linea.substr(pos0);
                linea=linea.substr(6);
            }
            linea=trim(linea);
            return linea;
            break;
        }

    }
    return "";
}

void delete_comments(ifstream &file, vector<string> &vs){
    file.seekg(0,ios::beg);
    size_t pos_com, pos_0, pos_1; // ERASE COMMENTS ( //  and  / *  * / )
    bool active_comment = 0;

    string linea;

    if(file.fail()){
        cout << "Error";
        std::exit(EXIT_FAILURE);
    }
    string aux;
    while(!file.eof()){
        getline(file,linea);
        size_t y=linea.find('(');
        if(linea.find('(')!=string::npos){
            linea=linea.erase(0,linea.find('(') + 1 );
        }
        else if(linea.find("input")==string::npos & linea.find("output")==string::npos & linea.find("module")!=string::npos ){
            linea=linea.erase(0);
        }


        /***************** ERASE COMMENTS ( // )******************************************************/
        pos_com = linea.find("//");
        if(pos_com!=string::npos) //valido?
        {
            linea=linea.substr(0,pos_com);
        }
        /***************** ERASE COMMENTS ( // )******************************************************/


        /************* ERASE COMMENTS ( / *  * / ) ***************************************************/
        pos_0=linea.find("/*");
        pos_1=linea.find("*/");

        if(pos_0 != string::npos)
        {
            if(pos_1 != string::npos)
            {
                //aux.erase(pos_0,pos_1 +2);
                linea=linea.substr(0,pos_0) + linea.substr(pos_1 + 2, linea.length());
            }
            else
            {
                linea.erase(pos_0); // ERASE UNCLOSED COMMENT / * ......
                while (pos_1 == string::npos){
                    getline(file,linea);
                    pos_1=linea.find("*/");
                }
                linea.erase(0,pos_1 + 2); // ERASE UNTIL  * / ......
            }
        }
        /************* ERASE COMMENTS ( / *  * / ) ***************************************************/


        linea=trim(linea);
        if( (linea != "") & ((linea.back()==')') | ((linea.back()==';'))) ){
            linea.erase(remove(linea.begin(), linea.end(), ')'), linea.end()); // REMOVE ')'
            linea.erase(remove(linea.begin(), linea.end(), ';'), linea.end()); // REMOVE ';'}
            if(linea != "") // CHECK IF IT IS EMPTY
            {
                vs.push_back(linea);
            }
            break;
        }
        if(linea != "") // // CHECK IF IT IS EMPTY
        {
            vs.push_back(linea);
        }
    }
}

vector <string> split_by_comma(vector <string> &vs){
    vector <string> results;
    for (auto words : vs) {
        stringstream  ss(words);
        string str;
        while (getline(ss, str, ',')) {
            results.push_back(str);
        }
    }
    return results;
}

void split_inputs_outputs(vector <string> &vs, vector <pair<string,int>> &bus_inputs, vector <pair<string,int>> &bus_outputs, vector <string> &singleBit_inputs, vector <string> &singleBit_outputs){
    string aux_type; // VALUE TYPE (input/ output)
    bool band_input; // FLAG
    int num1, num2;

    for(int i=0;i<vs.size();i++){
        vs[i]=trim(vs[i]);

        if(vs[i].find("input")!=string::npos){
            band_input=true;

            if( (vs[i].find('[')!=string::npos) & (vs[i].find(']')!=string::npos)) {

                aux_type = vs[i].substr(vs[i].find('['), vs[i].find(']')-vs[i].find('[')+1);
                aux_type.erase(remove(aux_type.begin(), aux_type.end(), ' '), aux_type.end()); //remueve espacios en blanco entre corchetes
                vs[i]=vs[i].substr(vs[i].find(']')+1);
                vs[i]=trim(vs[i]);
                num1 = stoi(aux_type.substr(aux_type.find('[')+1, aux_type.find(':')-aux_type.find('[')-1));
                num2 = stoi(aux_type.substr(aux_type.find(':')+1, aux_type.find(']')-aux_type.find(':')-1));

                if(num1 > num2){
                    bus_inputs.push_back(make_pair(vs[i],num1+1));
                }
                else{
                    bus_inputs.push_back(make_pair(vs[i],num2+1));
                }
            }

            else{
                aux_type="";
                vs[i]=vs[i].substr(vs[i].find('t')+1);
                vs[i]=trim(vs[i]);
                singleBit_inputs.push_back(vs[i]);
            }

        }
        else if( vs[i].find("output")!=string::npos ) {
            band_input=false;

            if( (vs[i].find('[')!=string::npos) & (vs[i].find(']')!=string::npos)) {

                aux_type = vs[i].substr(vs[i].find('['), vs[i].find(']')-vs[i].find('[')+1);
                aux_type.erase(remove(aux_type.begin(), aux_type.end(), ' '), aux_type.end()); //remueve espacios en blanco entre corchetes
                vs[i]=vs[i].substr(vs[i].find(']')+1);
                vs[i]=trim(vs[i]);
                num1 = stoi(aux_type.substr(aux_type.find('[')+1, aux_type.find(':')-aux_type.find('[')-1));
                num2 = stoi(aux_type.substr(aux_type.find(':')+1, aux_type.find(']')-aux_type.find(':')-1));

                if(num1 > num2){
                    bus_outputs.push_back(make_pair(vs[i],num1+1));
                }
                else{
                    bus_outputs.push_back(make_pair(vs[i],num2+1));
                }
            }

            else{
                aux_type="";
                vs[i]=vs[i].substr(vs[i].find("put")+3);
                vs[i]=trim(vs[i]);
                singleBit_outputs.push_back(vs[i]);
            }
        }
        else{
            if(aux_type==""){
                if(band_input){
                    singleBit_inputs.push_back(trim(aux_type + " " + vs[i]));
                }
                else{
                    singleBit_outputs.push_back(trim(aux_type + " " + vs[i]));
                }
            }
            else{
                if(band_input){
                    if(num1 > num2){
                        bus_inputs.push_back(make_pair(vs[i],num1+1));
                    }
                    else{
                        bus_inputs.push_back(make_pair(vs[i],num2+1));
                    }
                }
                else{
                    if(num1 > num2){
                        bus_outputs.push_back(make_pair(vs[i],num1+1));
                    }
                    else{
                        bus_outputs.push_back(make_pair(vs[i],num2+1));
                    }
                }
            }
        }
    }
}


///////////////////// CLASS DEFINITION FOR VERILOG MODULE //////////////////////////
class VerilogModule {

public:
    string name;
    vector <pair<string, int>> bus_inputs;
    vector <pair<string, int>> bus_outputs;
    vector <string> singleBit_inputs;
    vector <string> singleBit_outputs;

};

string CreateForLoop (string var, int num) {
    string forLoop = "\n";
    forLoop += "/*\n";
    forLoop += "  for ( i=0 ; i<2**" + to_string(num) + " ; i=i+1 )\n";
    forLoop += "  begin\n";
    forLoop += "     " + var + "_tb" +" = i; \n     #1;\n";
    forLoop += "  end\n";
    forLoop += "*/\n";
    return forLoop;
}


int main(){

    VerilogModule myModule1;

//////////////// READ MODULE AND ASSIGN DATA TO ARRAYS /////////////////////////
    ifstream  file;
    vector <string> vs;
    file.open("test.v",ios::in); //Open the file
    string name_module_tb = get_name_module(file); //get the name of the module
    myModule1.name = name_module_tb; //assign the name 
    delete_comments(file,vs);    // ERASE COMMENTS

    vs = split_by_comma(vs);    // SPLIT WITH COMMA EACH PORT
    split_inputs_outputs(vs, myModule1.bus_inputs, myModule1.bus_outputs, myModule1.singleBit_inputs, myModule1.singleBit_outputs); // PORTS SPLIT

    file.close();

//////////////// WRITE TEST BENCH INPUTS & OUTPUTS TO A FILE ////////////////

    string regs = "\n";
    string wires = "\n";
    string uut = "\n" + myModule1.name + " UUT( ";
    string assignations = "\n";
    string sbiConc = "{ "; //single bit inputs concatenated
    string busIterations;

    for (string auxString: myModule1.singleBit_inputs){
        regs += " reg\t" + auxString + "_tb;\n";
        uut += "\n\t\t\t." + auxString + "\t( " + auxString +"_tb ),";
        assignations += " " + auxString + "_tb\t=\t1'b0;\n";
        sbiConc += auxString + "_tb, ";
    }

    for (pair auxPair: myModule1.bus_inputs){
        regs += " reg [" + to_string(auxPair.second-1) + ":0]\t" + auxPair.first + "_tb;\n";
        uut += "\n\t\t\t." + auxPair.first + "\t( " + auxPair.first +"_tb ),";
        assignations += " " + auxPair.first + "_tb\t=\t" + to_string(auxPair.second) + "'b" + string(auxPair.second, '0') + ";\n";
        busIterations += CreateForLoop(auxPair.first, auxPair.second);
    }

    for (string auxString: myModule1.singleBit_outputs){
        wires += " wire\t" + auxString + "_tb;\n";
        uut += "\n\t\t\t." + auxString + "\t( " + auxString +"_tb ),";
    }

    for (pair auxPair: myModule1.bus_outputs){
        wires += " wire [" + to_string(auxPair.second-1) + ":0]\t" + auxPair.first + "_tb;\n";
        uut += "\n\t\t\t." + auxPair.first + "\t( " + auxPair.first +"_tb ),";
    }

    uut.pop_back();
    uut.pop_back();
    uut += ")  );\n";

    string sbiConcIterations;
    if (!myModule1.singleBit_inputs.empty()){
        sbiConc.pop_back();
        sbiConc.pop_back();
        sbiConc += " }";
        sbiConcIterations = CreateForLoop (sbiConc, myModule1.singleBit_inputs.size());
    }

    fstream myFile;
    myFile.open(myModule1.name + "_tb.sv", ios::out);
    if (myFile.is_open()) {

        myFile << "`timescale 1ns/1ns\n\n";
        myFile << "module " << myModule1.name + "_tb;\n";
        myFile << regs;
        myFile << wires;
        myFile << "\n integer i;\n";
        myFile << uut;
        myFile << "\ninitial begin\n";
        myFile << "\n $dumpvars( 1, " + myModule1.name + "_tb );";
        myFile << "\n $dumpfile( \"dump.vcd\" );\n";
        myFile << assignations;
        myFile << "\n #1;\n";
        myFile << sbiConcIterations;
        myFile << busIterations;
        myFile << "\n $finish();\n";
        myFile << "\n end";
        myFile << "\nendmodule";

        myFile.flush();
        myFile.close();
    }

    return 0;
}