#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <map>
#include <tuple>
#include "DynamicArray.h"

// ============ TIME AND DATE FUNCTIONS ============
// Returns today's date as a formatted string (DD-MM-YYYY)
std::string LiveDate(){
    time_t now = time(nullptr);
    tm* t = localtime(&now);

    std::string fecha =
        std::to_string(t->tm_mday) + "-" +
        std::to_string(t->tm_mon + 1) + "-" +
        std::to_string(t-> tm_year + 1900);

    return fecha;
}

// Returns the current month as an integer (1-12)
int LiveMonth(){
    time_t now = time(nullptr);
    tm* t = localtime(&now);

    int month = t->tm_mon + 1; 
    return month;
}

enum class TransactionDirection{in, out};

struct Transactions{
    int ID;
    char itemType[50];
    double amount;
    TransactionDirection type;
    char date[11]; 
    char note[100];
    char Category[100];
    int DateMonth;
    int FullDate;
    int year;
};

DynamicArray<Transactions>Transaction;
DynamicArray<Transactions>TransactionBackup;

// ============ BINARY FILE OPERATIONS ============
// Appends a single transaction record to the binary master file
void WSaveLogic(Transactions Struct){
    std::ofstream outfile("MasterFile.bin" , std::ios::binary | std::ios::app);
    if(!outfile.is_open()){
        std::cerr << "Sorry cannot open Master file! Exiting program." << std::endl;
    }
    else{
        outfile.write(reinterpret_cast<char*>(&Struct), sizeof(Struct));
    }
}

// Overwrites the entire binary master file with all current transactions (used after modifications)
void DWSaveLogic(){

    std::ofstream outfile("MasterFile.bin" , std::ios::binary | std::ios::trunc);
    if(!outfile.is_open()){
        std::cerr << "Sorry cannot open Master file! Exiting program." << std::endl;
    }
    else{
        for (int i = 0; i < Transaction.size(); i++){
            outfile.write(reinterpret_cast<char*>(&Transaction[i]), sizeof(Transactions));
        }
        
    }

}

std::string format_money(double v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << v;

    std::string s = oss.str();
    auto dot = s.find('.');
    int i = (int)dot - 3;

    while (i > 0) {
        s.insert(i, ",");
        i -= 3;
    }

    return s;
}

// Displays all transactions in the Transaction array with full details
void DataDisplay(){
    if (Transaction.size() == 0){
        std::cout << "No transactions to display." << std::endl;
        return;
    }
    
    std::cout << "\n";
    std::cout << "╔═════╦════════════════════════════════════════════════════════════════╦═══════════╦════════╦════════════╦════════════════╦═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║ ID  ║   Item                                                         ║  Amount   ║  Type  ║    Date    ║    Category    ║   Note                                                                                                    ║\n";
    std::cout << "╠═════╬════════════════════════════════════════════════════════════════╬═══════════╬════════╬════════════╬════════════════╬═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
                                                                                                                                                                                                                        
    for (const Transactions& n : Transaction){            
        std::cout << "║" << std::setw(4) << std::right << n.ID << " ║"
                  << std::setw(49.9) << std::left << n.itemType << "               ║"
                  << std::setw(10) << std::left << std::fixed << std::setprecision(2) << "£" << format_money(n.amount) << " ║"
                  << std::setw(8.5) << std::left << (n.type == TransactionDirection::in ? "IN" : "OUT") << "║"
                  << std::setw(12) << std::left << n.date << "║"
                  << std::setw(16) << std::left << n.Category << "║"
                  << std::setw(107) << std::left << n.note << "║\n";
    }
    
    std::cout << "╚═════╩════════════════════════════════════════════════════════════════╩═══════════╩════════╩════════════╩════════════════╩═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝ \n";
    std::cout << "\n";
}

// Writes the TransactionBackup array to the backup binary file
void BWSaveLogic(){
    std::ofstream outfile("BackupMasterFile.bin" , std::ios::binary | std::ios::trunc);
    if(!outfile.is_open()){
        std::cerr << "Sorry cannot open Master file! Exiting program." << std::endl;
    }
    else{
        for (int i = 0; i < TransactionBackup.size(); i++){
            outfile.write(reinterpret_cast<char*>(&TransactionBackup[i]), sizeof(Transactions));
        }
    }

}

// Reads all records from the backup binary file into the TransactionBackup array
void BRSaveLogic(Transactions &Struct){
    std::ifstream infile("BackupMasterFile.bin", std::ios::binary);
    if(!infile.is_open()){
        std::cerr << "Sorry cannot open Backup Master file! Exiting program." << std::endl;
    }
    else{
        while(infile.read(reinterpret_cast<char*>(&Struct), sizeof(Struct))){
            TransactionBackup.push_back(Struct);
        }
    }
}

// Reads all records from the master binary file into the Transaction array
void RSaveLogic(Transactions &Struct){
    std::ifstream infile("MasterFile.bin" , std::ios::binary);
    if(!infile.is_open()){
        std::cerr << "Sorry cannot open Master file! Exiting program." << std::endl;
    }
    else{
        while(infile.read(reinterpret_cast<char*>(&Struct), sizeof(Struct))){
            Transaction.push_back(Struct);
        }
    }

}

// Saves the current ID counter to a text file for persistence
void IDtracker(int IDTic){
    std::ofstream IDTracker("IDtracker.txt" , std::ios::trunc);
        if (!IDTracker.is_open()){
        std::cout << "Sorry cannot open Tracker file! Exiting program." << std::endl;
    }
    else{
        IDTracker << IDTic << std::endl;
    }
    
}

bool ismoney(const std::string& s){
    if (s.empty()) return false;

    bool seenDOT = false;
    int decimals = 0;

    for (char c : s) {
        if (c == ',') continue;              // allow commas anywhere (display concern)

        if (c == '.') {
            if (seenDOT) return false;
            seenDOT = true;
            continue;
        }

        if (!std::isdigit((unsigned char)c)) return false;

        if (seenDOT) {
            ++decimals;
            if (decimals > 2) return false;  // max 2 decimal places
        }
    }

    return true;
}

std::string strip_commas(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ','), s.end());
    return s;
}
bool isdatevalid(const std::string& s){
    // Accept formats: DD/MM/YYYY or DD-MM-YYYY (10 characters)
    if (s.size() != 10) return false;
    if (!std::isdigit((unsigned char)s[0]) || !std::isdigit((unsigned char)s[1])) return false;
    char sep = s[2];
    if (sep != '/' && sep != '-') return false;
    if (!std::isdigit((unsigned char)s[3]) || !std::isdigit((unsigned char)s[4])) return false;
    if (s[5] != sep) return false;
    if (!std::isdigit((unsigned char)s[6]) || !std::isdigit((unsigned char)s[7]) || !std::isdigit((unsigned char)s[8]) || !std::isdigit((unsigned char)s[9])) return false;

    int day = (s[0]-'0')*10 + (s[1]-'0');
    int mon = (s[3]-'0')*10 + (s[4]-'0');
    int year = (s[6]-'0')*1000 + (s[7]-'0')*100 + (s[8]-'0')*10 + (s[9]-'0');

    if (day < 1 || day > 31) return false;
    if (mon < 1 || mon > 12) return false;
    if (year < 1000 || year > 9999) return false;

    return true;
}
// ============ CSV IMPORT/EXPORT FUNCTIONS ============
// Exports all transactions to a CSV file for external use or backup
void CSVExporter(){
    std::ofstream CSV("Exported_Records.csv" , std::ios::trunc);
    if (!CSV.is_open()){
        std::cerr << "Sorry cannot open file! Exiting program." << std::endl;
    }
    else{
        CSV << "ID,Item,Amount,Type,Date,Note,Category\n";
                // Write each transaction
        for (int i = 0; i < Transaction.size(); i++){
            const Transactions& t = Transaction[i];
            CSV << t.ID << ","
                << "\"" << t.itemType << "\"" << ","
                << t.amount << ","
                << (t.type == TransactionDirection::in ? "INBOUND" : "OUTBOUND") << ","
                << t.date << ","
                << "\"" << t.note << "\"" << ","
                << "\"" << t.Category << "\"" << "\n";
        }
    }
}

// ============ DATE PARSING ============
// Parses a date string (DD-MM-YYYY) into its components and returns as a tuple
std::tuple<int,int,int> ParseDate(const std::string a){
    std::tuple<int,int,int> dateExtractor; 
    size_t d1 = a.find('/');
    if (d1 == std::string::npos) {
        d1 = a.find('-');
    }
    size_t d2 = a.find('/', d1 + 1);
    if (d2 == std::string::npos) {
        d2 = a.find('-', d1 + 1);
    }   

    int day   = std::stoi(a.substr(0, d1));
    int month = std::stoi(a.substr(d1 + 1, d2 - (d1 + 1)));
    int year  = std::stoi(a.substr(d2 + 1));

    return {day, month , year};
}


// Reads the current ID counter from the ID tracker file
int readID(){
    std::ifstream file("IDtracker.txt");
    int A = 0;
    file >> A;
    return A;
}

// Safely converts a string to a fixed-size char array with null termination
void StringToChar(std::string Target , int Limit , char* PTC){
    size_t Length = std::min(Target.size(),(size_t)Limit);//THis ensures the date does not exceed the char array limit.
    std::copy(Target.begin(),Target.begin() + Length , PTC);// Copy date string to char array
    PTC[Length] = '\0'; //THis tells where the string ends.
}

// Validates CSV lines by counting fields (commas + 1)
int CountFields(const std::string& line){
    int commaCount = 0;
    for (char c : line){
        if (c == ','){
            commaCount++;
        }
    }
    return commaCount + 1; // Number of fields = comma count + 1
}

// Imports transactions from a CSV file, validates each line, and stores in Transaction array
void CSVImporter(){
    std::ifstream CSV;
    CSV.open("Exported_Records.csv");
    if (!CSV.is_open()){
        std::cerr << "Sorry cannot open file! Exiting program." << std::endl;
    }
    else{
        std::string line;
        bool isFirstLine = true;
        int skippedLines = 0;
        while (std::getline(CSV, line)){
            if (isFirstLine) {
                isFirstLine = false;
                continue;  // Skip the header row
            }
            
            // Validate that the line has at least 7 fields (6 commas)
            if (CountFields(line) < 7){
                std::cerr << "Skipping invalid line (insufficient fields): " << line << std::endl;
                skippedLines++;
                continue;
            }
            
            Transactions tempRecord;
            std::stringstream ss(line);
            std::string field;
            int read = 0;
            // Parse ID
            std::getline(ss, field, ',');
            tempRecord.ID = std::stoi(field);
            
            // Parse Item (itemType)
            std::getline(ss, field, ',');
            StringToChar(field, 50, tempRecord.itemType);
            
            // Parse Amount
            std::getline(ss, field, ',');
            tempRecord.amount = std::stod(field);
            
            // Parse Type (INBOUND/OUTBOUND)
            std::getline(ss, field, ',');
            if (field == "INBOUND"){
                tempRecord.type = TransactionDirection::in;
            }
            else if (field == "OUTBOUND"){
                tempRecord.type = TransactionDirection::out;
            }
            
            // Parse Date
            std::getline(ss, field, ',');
            StringToChar(field, 10, tempRecord.date);
            
            // Extract date components for month and full date
            auto [day, month, year] = ParseDate(field);
            tempRecord.DateMonth = month;
            tempRecord.year = year;
            tempRecord.FullDate = (year * 10000) + (month * 100) + day;
            
            // Parse Note
            std::getline(ss, field, ',');
            StringToChar(field, 100, tempRecord.note);
            
            // Parse Category
            std::getline(ss, field, ',');
            StringToChar(field, 100, tempRecord.Category);
            
            // Add the parsed record to the Transaction array
            Transaction.push_back(tempRecord);
            
            std::cout << "Imported record ID: " << tempRecord.ID << " - " << tempRecord.itemType << std::endl;
        }
        
        // Update ID tracker with the highest ID
        if (Transaction.size() > 0){
            IDtracker(Transaction.size());
        }
        
        std::cout << "Successfully imported " << Transaction.size() << " records from CSV.";
        if (skippedLines > 0){
            std::cout << " (" << skippedLines << " invalid lines skipped)";
        }
        std::cout << std::endl;
        DWSaveLogic();  // Save imported records to the binary file
    }
}
// Creates a backup copy of all current transactions for undo functionality
void BackupLogic(){
    TransactionBackup.clear();
    for(int i = 0; i < Transaction.size(); i++){
            TransactionBackup.push_back(Transaction[i]);
    }
    BWSaveLogic();
}

// ============ TRANSACTION MANAGEMENT ============
// Class that handles adding a new transaction to the system and saving it
class AddTransaction{
    public:
    int ID;
    std::string Item;
    double amount;
    TransactionDirection status;
    std::string Date;
    std::string Note;
    int MonthValue;
    int DateFull;
    std::string category;
    int year;

    AddTransaction(int id ,std::string Xitem ,double amt , TransactionDirection TDS, std::string d ,std::string group, std::string n , int month , int Idate , int Year)
    :ID(id) ,Item(Xitem), amount(amt)  , Date(d) , status(TDS) ,Note(n) , MonthValue(month) , DateFull(Idate) , category(group) ,year(Year)
    {   
        BackupLogic();
        Transactions tempRecord; // Create a temporary Transactions object
        //Takes user data and assigns it to the tempRecord object before pushing it to the DynamicArray.
        tempRecord.ID = id;
        tempRecord.amount = amt;
        tempRecord.type = TDS;
        tempRecord.DateMonth = month;
        tempRecord.FullDate = Idate;
        tempRecord.year = Year;

        StringToChar(group,100,tempRecord.Category);
        StringToChar(d,10,tempRecord.date);
        StringToChar(n,100,tempRecord.note);
        StringToChar(Xitem,50,tempRecord.itemType);
        Transaction.push_back(tempRecord);
        WSaveLogic(tempRecord);
    }
};
// Deletes a transaction by ID after user confirmation, updates all IDs, and saves changes
int Delete(int A){
    std::string choice;
    std::cout << "Are you sure [Y/n]: ";
    std::cin >> choice;
    while(true){
        if(choice == "Y"|| choice == "y"){
            BackupLogic(); 
            BWSaveLogic();
            Transaction.erase(A-1);
            int IDsub = readID();
            IDsub -= 1;
            IDtracker(IDsub);
            for (int j = 0; j < Transaction.size(); j++) {
                Transaction[j].ID = j + 1;
            }
            DWSaveLogic();
            return 0;
        }
        else if (choice == "N"|| choice == "n"){
            return 0;
        }
        else{
            std::cout << "Please choose a valid otpion" << std::endl;
        }
    }
    return 0;
}
// ============ VIEWING AND REPORTING ============
// Main function for viewing transactions with various filters (-list, -report by month/year, -after date, -group by category)
int View(int argc, char* argv[]){
    if (argc < 2 || argv[1] == nullptr){
        return 1;
    }

    std::string Pilot = argv[1];
    if (Pilot == "-list" && argc == 2){
        DataDisplay();
        return 0;
    }

    if (argc < 4 || argv[2] == nullptr || argv[3] == nullptr){
        std::cerr << "Error: -list requires a filter and value, e.g. -list month 5\n";
        return 1;
    }

    std::string Filter = argv[2];
    std::string FilterNo = argv[3];

    if (Filter == "month"){
        std::string reportID = argv[3];
        int MonthIndicator = stoi(reportID);
        if (MonthIndicator >= 1 && MonthIndicator <= 12){
            std::string yearID = argv[4];
            int Year = stoi(yearID);
            if (Year >= 0000 && Year<= 10000){
                std::cout << "\n";
                std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
                std::cout << "║" << std::setw(58) << std::left << ("        RECORDS IN MONTH" + reportID) << " ║\n";
                std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
                std::cout << "╔═════╦════════════════════════════════════════════════════════════════╦═══════════╦════════╦════════════╦════════════════╦═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ ID  ║   Item                                                         ║  Amount   ║  Type  ║    Date    ║    Category    ║   Note                                                                                                    ║\n";
                std::cout << "╠═════╬════════════════════════════════════════════════════════════════╬═══════════╬════════╬════════════╬════════════════╬═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
                for (const Transactions& n: Transaction){
                        if (n.DateMonth == MonthIndicator){
                        std::cout << "║" << std::setw(4) << std::right << n.ID << " ║"
                        << std::setw(49.9) << std::left << n.itemType << "               ║"
                        << std::setw(5) << std::left << std::fixed << std::setprecision(2) << "£" << format_money(n.amount) << " ║"
                        << std::setw(8.5) << std::left << (n.type == TransactionDirection::in ? "IN" : "OUT") << "║"
                        << std::setw(12) << std::left << n.date << "║"
                        << std::setw(16) << std::left << n.Category << "║"
                        << std::setw(107) << std::left << n.note << "║\n";
                }
                }
            std::cout << "╚═════╩════════════════════════════════════════════════════════════════╩═══════════╩════════╩════════════╩════════════════╩═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝ \n";
            std::cout << "\n";
            }
        }
    }

    if (Filter == "year"){
        std::string yearID = argv[3];
        int Year = stoi(yearID);
        if (Year >= 0000 && Year<= 10000){
            std::cout << "\n";
            std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
            std::cout << "║" << std::setw(58) << std::left << ("        RECORDS IN " + yearID) << " ║\n";
            std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
            std::cout << "╔═════╦════════════════════════════════════════════════════════════════╦═══════════╦════════╦════════════╦════════════════╦═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║ ID  ║   Item                                                         ║  Amount   ║  Type  ║    Date    ║    Category    ║   Note                                                                                                    ║\n";
            std::cout << "╠═════╬════════════════════════════════════════════════════════════════╬═══════════╬════════╬════════════╬════════════════╬═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
            for (const Transactions& n: Transaction){
                    if (n.year == Year){
                        std::cout << "║" << std::setw(4) << std::right << n.ID << " ║"
                        << std::setw(49.9) << std::left << n.itemType << "               ║"
                        << std::setw(5) << std::left << std::fixed << std::setprecision(2) << "£" << format_money(n.amount) << " ║"
                        << std::setw(8.5) << std::left << (n.type == TransactionDirection::in ? "IN" : "OUT") << "║"
                        << std::setw(12) << std::left << n.date << "║"
                        << std::setw(16) << std::left << n.Category << "║"
                        << std::setw(107) << std::left << n.note << "║\n";
                    }
            }
            std::cout << "╚═════╩════════════════════════════════════════════════════════════════╩═══════════╩════════╩════════════╩════════════════╩═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝ \n";
            std::cout << "\n";
        }
    }
    

    if (Filter == "after" && FilterNo == argv[3]){
        auto [day, month, year] = ParseDate(FilterNo);
        int date = (year * 10000) + (month * 100) + day;
        std::cout << date << std::endl;
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║" << std::setw(58) << std::left << ("        RECORDS AFTER " + FilterNo) << " ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << "╔═════╦════════════════════════════════════════════════════════════════╦═══════════╦════════╦════════════╦════════════════╦═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ ID  ║   Item                                                         ║  Amount   ║  Type  ║    Date    ║    Category    ║   Note                                                                                                    ║\n";
        std::cout << "╠═════╬════════════════════════════════════════════════════════════════╬═══════════╬════════╬════════════╬════════════════╬═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
            for (const Transactions& n : Transaction){
                if (n.FullDate >= date){
                        std::cout << "║" << std::setw(4) << std::right << n.ID << " ║"
                        << std::setw(49.9) << std::left << n.itemType << "               ║"
                        << std::setw(5) << std::left << std::fixed << std::setprecision(2) << "£" << format_money(n.amount) << " ║"
                        << std::setw(8.5) << std::left << (n.type == TransactionDirection::in ? "IN" : "OUT") << "║"
                        << std::setw(12) << std::left << n.date << "║"
                        << std::setw(16) << std::left << n.Category << "║"
                        << std::setw(107) << std::left << n.note << "║\n";
                }
            }
        std::cout << "╚═════╩════════════════════════════════════════════════════════════════╩═══════════╩════════╩════════════╩════════════════╩═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝ \n";
        std::cout << "\n";
    }

    if (Filter == "group"){
        std::string FilterStr = argv[3];
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║" << std::setw(58) << std::left << ("        RECORDS IN " + FilterStr) << "║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "╔═════╦════════════════════════════════════════════════════════════════╦═══════════╦════════╦════════════╦════════════════╦═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ ID  ║   Item                                                         ║  Amount   ║  Type  ║    Date    ║    Category    ║   Note                                                                                                    ║\n";
        std::cout << "╠═════╬════════════════════════════════════════════════════════════════╬═══════════╬════════╬════════════╬════════════════╬═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
        for (const Transactions& n : Transaction){
            if (n.Category == FilterStr){
                std::cout << "║" << std::setw(4) << std::right << n.ID << " ║"
                << std::setw(49.9) << std::left << n.itemType << "               ║"
                << std::setw(5) << std::left << std::fixed << std::setprecision(2) <<"£" << format_money(n.amount) << " ║"
                << std::setw(8.5) << std::left << (n.type == TransactionDirection::in ? "IN" : "OUT") << "║"
                << std::setw(12) << std::left << n.date << "║"
                << std::setw(16) << std::left << n.Category << "║"
                << std::setw(107) << std::left << n.note << "║\n";
            }
        }
        std::cout << "╚═════╩════════════════════════════════════════════════════════════════╩═══════════╩════════╩════════════╩════════════════╩═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝ \n";
        std::cout << "\n";   
    }

    
    return 0;
}

void Report(int argc, char* argv[]){
    std::string Filter = argv[2];
    if (Filter == "month"){
        std::string reportID = argv[3];
        const int labelwidth = 14;
        const int barwidth = 20;
        const double unit = 5.0; 
        std::cout << std::fixed << std::setprecision(2);
        int MonthIndicator = stoi(reportID);
        if (MonthIndicator >= 1 && MonthIndicator <= 12){
            std::string yearID = argv[4];
            int Year = stoi(yearID);
            if (Year >= 0000 && Year<= 10000){
            std::map<std::string,double> GrandTotal;
            double TotalSum = 0.0;

            for(const auto& n : Transaction){
                if (n.DateMonth == MonthIndicator && n.year == Year && n.type == TransactionDirection::out){
                    GrandTotal[n.Category] += n.amount;
                    TotalSum += n.amount;
                }
            }
            
            std::cout << "\n";
            std::cout << "╔══════════════════════════════════════════════════════════╗\n";
            std::cout << "║           MONTHLY EXPENSE BREAKDOWN REPORT               ║\n";
            std::cout << "╠══════════════════════════════════════════════════════════╣\n";
            
            for (const auto& [cat , total] : GrandTotal){
                int filled = static_cast<int>(std::floor(total/unit));
                if (filled < 0) filled = 0;
                if (filled > barwidth) filled = barwidth;

                std::cout << "║ " << std::left << std::setw(labelwidth) << cat << " │";
                for (int i = 0; i < filled ; i++) std::cout << '=';
                for (int i = filled; i < barwidth; ++i) std::cout << ' ';
                std::cout << "│ £" << std::right << std::setw(10) << total << "        ║\n";
            }

            double ITotalSum = 0.0;

            for(const auto& n : Transaction){
                if (n.DateMonth == MonthIndicator && n.type == TransactionDirection::in && n.year == Year){
                        GrandTotal[n.Category] += n.amount;
                        ITotalSum += n.amount;
                }
            }
            std::cout << "╠══════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Month Expenses:£" << std::left << std::setw(41) << format_money(TotalSum) << "║\n";
            std::cout << "║ Income:        £" << std::left << std::setw(41) << format_money(ITotalSum) << "║\n";
            double OverallTotal = ITotalSum - TotalSum;
            std::cout << "║ Overall:       £" << std::left << std::setw(41) << format_money(OverallTotal) << "║\n";
            std::cout << "╚══════════════════════════════════════════════════════════╝\n";
            std::cout << "\n";
            }
        }
    }

    if (Filter == "year"){
        std::string YID = argv[3];
        int yearIndicator = stoi(YID);
        const int labelwidth = 14;
        const int barwidth = 20;
        const double unit = 5.0; 
        if (yearIndicator >= 0000 && yearIndicator <= 10000){
            std::map<std::string,double> GrandTotal;
            double TotalSum = 0.0;
            for(const auto& n : Transaction){
                if (n.year == yearIndicator && n.type == TransactionDirection::out){
                    GrandTotal[n.Category] += n.amount;
                    TotalSum += n.amount;
                }
            }
            
            std::cout << "\n";
            std::cout << "╔════════════════════════════════════════════════════════════╗\n";
            std::cout << "║            YEARLY EXPENSE BREAKDOWN REPORT                 ║\n";
            std::cout << "╠════════════════════════════════════════════════════════════╣\n";
            
            for (const auto& [cat , total] : GrandTotal){
                int filled = static_cast<int>(std::floor(total/unit));
                if (filled < 0) filled = 0;
                if (filled > barwidth) filled = barwidth;

                std::cout << "║ " << std::left << std::setw(labelwidth) << cat << " │";
                for (int i = 0; i < filled ; i++) std::cout << '=';
                for (int i = filled; i < barwidth; ++i) std::cout << ' ';
                std::cout << "│ £ " << std::right << std::setw(10) << std::fixed << std::setprecision(2) << total << "         ║\n";
            }

            double ITotalSum = 0.0;

            for(const auto& n : Transaction){
                if (n.year == yearIndicator && n.type == TransactionDirection::in){
                        GrandTotal[n.Category] += n.amount;
                        ITotalSum += n.amount;
                }
            }
            std::cout << "╠════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Year Expenses:£" << std::left << std::setw(41) << format_money(TotalSum) << "║\n";
            std::cout << "║ Income:        £" << std::left << std::setw(41) << format_money(ITotalSum) << "║\n";
            double OverallTotal = ITotalSum - TotalSum;
            std::cout << "║ Overall:       £" << std::left << std::setw(41) << format_money(OverallTotal) << "║\n";
            std::cout << "╚════════════════════════════════════════════════════════════╝\n";
            std::cout << "\n";
        }
    }
}

// ============ UNDO FUNCTIONALITY ============
// Restores the previous state of transactions from the backup file
int Undo(){
    TransactionBackup.clear();
    Transactions temp;  
    BRSaveLogic(temp);
    Transaction.clear();
    for(int i = 0;i < TransactionBackup.size(); i++){
        Transaction.push_back(TransactionBackup[i]);
    }

    for (int j = 0; j < Transaction.size(); j++) {
        Transaction[j].ID = j + 1;
    }
    IDtracker(Transaction.size());
    DWSaveLogic();
    return 0;
}

// ============ BALANCE REPORTING ============
// Calculates and displays the current month's balance (income - expenses)
int Balance(){
    double OverallTotal;
    int MonthIndicator = LiveMonth();
    if (MonthIndicator >= 1 && MonthIndicator <= 12){
        std::map<std::string,double> GrandTotal;
        double ITotalSum = 0.0;
        double TotalSum = 0.0;

        for(const auto& n : Transaction){
            if (n.DateMonth == MonthIndicator && n.type == TransactionDirection::in){
                    GrandTotal[n.Category] += n.amount;
                    ITotalSum += n.amount;
            }
        }
        GrandTotal.clear();

        for(const auto& n : Transaction){
            if (n.DateMonth == MonthIndicator && n.type == TransactionDirection::out){
                GrandTotal[n.Category] += n.amount;
                TotalSum += n.amount;
            }
        }

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              CURRENT MONTH BALANCE REPORT                  ║\n";
        std::cout << "╠════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Month Expenses: £" << std::left << std::setw(42) << std::fixed << std::setprecision(2) << format_money(TotalSum) << "║\n";
        std::cout << "║ Income:         £" << std::left << std::setw(42) << std::fixed << std::setprecision(2) << format_money(ITotalSum) << "║\n";
        std::cout << "╠════════════════════════════════════════════════════════════╣\n";
        OverallTotal = ITotalSum - TotalSum;
        if (OverallTotal >= 0){
            std::cout << "║ Overall (Surplus): £" << std::left << std::setw(39) << std::fixed << std::setprecision(2) << format_money(OverallTotal) <<"║\n";
        } else {
            std::cout << "║ Overall (Deficit):  £" << std::left << std::setw(39) << std::fixed << std::setprecision(2) << format_money(OverallTotal) << "║\n";
        }
        std::cout << "╚════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
    }

    return OverallTotal;
}
// ============ UTILITY FUNCTIONS ============
// Displays the help menu with all available commands and usage examples
void Help(){
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          ONE-SHOT INCOME & EXPENSE TRACKER - HELP MENU         ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    std::cout << "┌─ ADD TRANSACTION ───────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -add \"<item>\" <amount> <type> <date> <category> <note>                  │\n";
    std::cout << "│ • item:     Name of the transaction (RECOMMENDED: use \"\" for item)              │\n";
    std::cout << "│ • amount:   Amount in £ (e.g., 50.00)                                           │\n";
    std::cout << "│ • type:     'in' for income, 'out' for expense                                  │\n";
    std::cout << "│ • date:     DD-MM-YYYY or 'd' for today's date                                  │\n";
    std::cout << "│ • category: Category (e.g., Food, Salary, Entertainment)                        │\n";
    std::cout << "│ • note:     Description (multiple words supported)                              │\n";
    std::cout << "│ Example: ./main -add \"Weekly Groceries\" 50.00 out d Food Shopping trip        │\n";
    std::cout << "│ NOTE: Using \"\" prevents undefined behaviors from spaces in item names          │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ VIEW TRANSACTIONS ─────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -list [filter] [value]                                 │\n";
    std::cout << "│ • -list                    View all transactions                │\n";
    std::cout << "│ • -list month <1-12>       View transactions for a month        │\n";
    std::cout << "│ • -list after <DD-MM-YYYY> View transactions after a date       │\n";
    std::cout << "│ • -list group <category>   View transactions by category        │\n";
    std::cout << "│ Examples:                                                       │\n";
    std::cout << "│   ./main -list                                                  │\n";
    std::cout << "│   ./main -list month 1                                          │\n";
    std::cout << "│   ./main -list after 01-01-2026                                 │\n";
    std::cout << "│   ./main -list group Food                                       │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ REPORTS ───────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -report <type> <value>                                             │\n";
    std::cout << "│ • -report month <1-12> <year>  Monthly report with income/expenses          │\n";
    std::cout << "│ • -report year <YYYY>          Yearly report with breakdown                 │\n";
    std::cout << "│ Examples:                                                                   │\n";
    std::cout << "│   ./main -report month 1 2026                                               │\n";
    std::cout << "│   ./main -report year 2026                                                  │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ DELETE TRANSACTION ──────────────────────────────────────────┐\n";
    std::cout << "│ Command: -delete <ID>                                         │\n";
    std::cout << "│ • ID: The transaction ID (shown when viewing transactions)    │\n";
    std::cout << "│ Example: ./main -delete 5                                     │\n";
    std::cout << "└───────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ BALANCE ────────────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -balance                                            │\n";
    std::cout << "│ Shows current month's income, expenses, and balance          │\n";
    std::cout << "│ Example: ./main -balance                                     │\n";
    std::cout << "└──────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ EXPORT TO CSV ──────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -export                                             │\n";
    std::cout << "│ Exports all transactions to 'Exported_Records.csv'           │\n";
    std::cout << "│ Example: ./main -export                                      │\n";
    std::cout << "└──────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ IMPORT FROM CSV ────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -import                                             │\n";
    std::cout << "│ Imports transactions from 'Exported_Records.csv'             │\n";
    std::cout << "│ Example: ./main -import                                      │\n";
    std::cout << "└──────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ UNDO ──────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -undo                                                  │\n";
    std::cout << "│ Restores the previous state of all transactions                 │\n";
    std::cout << "│ Example: ./main -undo                                           │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
    
    std::cout << "┌─ HELP ──────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Command: -h                                                     │\n";
    std::cout << "│ Displays this help menu                                         │\n";
    std::cout << "│ Example: ./main -h                                              │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
}

// Joins multiple command-line arguments into a single string (useful for multi-word descriptions)
std::string join_args(int start, int argc, char* argv[]){
    std::string result;
    for (int i = start; i < argc; ++i) {
        result += argv[i];
        if (i + 1 < argc)
            result += " ";
    }
    return result;
} 

int AddValidation(int argc, char* argv[]){
    if (argc < 7)
    {
        std::cerr << "Error: -add requires more data please refer to the manual '-h'.\n";
        return 1;
    }
    
    TransactionDirection status;
    std::string ItemType = argv[2];
    std::string amount = argv[3];
    std::string Placeholder = argv[4];
    std::string dateToken = argv[5];
    std::string category = argv[6];
    if (ItemType.empty()||ItemType.length() >= 50){
        std::cerr << "Please dont leave it blank and keep record name under 49 characters" << std::endl;
        return 1;
    }

    if (!ismoney(amount)) {
        std::cerr << "Amount must be a number like 1099.99. Got: " << amount << "\n";
        return 1;
    }

    if (dateToken != "d" && !isdatevalid(dateToken)) {
        std::cerr << "Date must be 'd' or DD/MM/YY (e.g. 23/02/25). Got: " << dateToken << "\n";
        return 1;
    }

    if (category.empty()){
        std::cerr << "Category cannot be empty.\n";
        return 1;
    }   
    return 0;
}
// ============ ADD TRANSACTION ============
// Processes command-line arguments to add a new transaction to the system
int Add(int argc, char* argv[], int ID){
        ID++;
        IDtracker(ID);
        std::string Notes;
        TransactionDirection status;
        std::string ItemType = argv[2];

        std::string amount = argv[3];
        amount = strip_commas(amount);
        double IAmount = stod(amount);

        std::string Placeholder = argv[4];
        if (Placeholder == "in"){
        status = TransactionDirection::in;
        }
        else if (Placeholder == "out"){
            status = TransactionDirection::out;
        }
        else{
            std::cerr << "Error! Input recieved: '" << Placeholder << "'. 'in' and 'out' are only valid for transaction types.\nRefer to the manual if needed '-h'." << std::endl;
            return 1;
        }

        std::string dateToken = argv[5];
        int FullIntDate;
        if (dateToken == "d"){
            dateToken = LiveDate();
            auto [day, DateMonth, YearExtractor] = ParseDate(dateToken);
            FullIntDate = (YearExtractor * 10000) + (DateMonth * 100) + day;

        }

        auto [day, DateMonth, YearExtractor] = ParseDate(dateToken);
        FullIntDate = (YearExtractor * 10000) + (DateMonth * 100) + day;

        std::string category = argv[6];
        std::string description = join_args(7, argc, argv);
        Notes = description;

        std::cout << "Argument count : " << argc << std::endl;
        for (int i = 0; i < argc; i++){
            std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
        }
        AddTransaction(ID,ItemType,IAmount,status,dateToken,category,Notes,DateMonth ,FullIntDate, YearExtractor);
    return 1;
}
void ExportToCSV(){
    CSVExporter();
    std::cout << "Sucessfully exported to CSV." << std::endl;
}

void ImportFromCSV(){
    CSVImporter();
}

// ============ MAIN PROGRAM ENTRY ============
// Command dispatcher: loads data from disk and routes to appropriate function based on user command
int main(int argc, char* argv[]){
    Transactions temp;
    RSaveLogic(temp);

    int IDParent = 0;
    IDParent = readID();

    //argc = number of command line arguments.
    //argv = array of command line arguments.

    
    if (argc < 2)
    {
        std::cerr << "Error: No valid command provided.\n";
        std::cerr << "Use -h for command help.\n";
        return 1;
    }

    std::string cmd = argv[1];
    if (cmd == "-add"){
        if(AddValidation(argc, argv) !=0) return 1;
        Add(argc,argv,IDParent);
    }

    else if (cmd == "-delete"){
        std::string TDselection = argv[2];
        int IDselection = stoi(TDselection);
        Delete(IDselection);
    }

    else if (cmd == "-list"){
        View(argc,argv);
    }

    else if (cmd == "-report"){
        Report(argc, argv);
    }

    else if (cmd == "-undo"){
        Undo();
    }

    else if (cmd == "-balance"){
        Balance();
    }

    else if (cmd == "-export"){
        ExportToCSV();
    }

    else if (cmd == "-import"){
        ImportFromCSV();
    }

    else if (cmd == "-h"){
        Help();
    }

    else{
        std::cerr << "Please choose a valid option or refer to the manual '-h'." << std::endl;
    }

    return 1;

}
