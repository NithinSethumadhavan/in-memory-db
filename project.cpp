#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

class Table {
 public:
  Table(const string& name, const vector<string>& column_names, const vector<vector<string> > data) : name_(name), column_names_(column_names), data_(data) {}

  const string& name() const { return name_; }
  const vector<string>& column_names() const { return column_names_; }
  const vector<vector<string> >& data() const { return data_; }

  // Callee will own the returned pointer
  Table* Select(const vector<string>& column_names) const {
    if(column_names.size() == 1 && column_names[0] == "*"){
        return new Table("Select", column_names_, data_);
    }
    const int data_count = data_.size();
    const int new_column_count = column_names.size();
    vector<int> column_index(new_column_count);


    vector<vector<string>> new_data(data_count, vector<string>(new_column_count));

    int k = 0;

    for(int i = 0; i < new_column_count; ++i)
    {
       int index = distance(column_names_.begin(), find(column_names_.begin(), column_names_.end(), column_names[i])); 
       column_index[k++] = index;
    }

    for(int i = 0; i < data_count; ++i)
    {
        k = 0;
        for(int j : column_index)
        {
            new_data[i][k++] = data_[i][j];
        }
    }
                
    return new Table("Select", column_names, new_data);
  }

  // Callee will own the returned pointer
  Table* Where(const string& column_name, const string& value) const 
  {
    const int column_index = distance(column_names_.begin(), 
        find(column_names_.begin(), column_names_.end(), column_name));
    
    vector<vector<string>> new_data;
    for(const vector<string>& row : data_)
    {
      if(row[column_index] == value)
      {
        new_data.push_back(row);
      }
    }

    return new Table("WhereTable", column_names_, new_data);
  }

  void Print() {
    string output = JoinStringVector(column_names_) + "\n";
    for (size_t row_index = 0; row_index < data_.size(); ++row_index) {
      output += JoinStringVector(data_[row_index]);
      output += "\n";
    }
    cout << output << endl;
  }

 private:
  string JoinStringVector(const vector<string>& input) {
    string output = "";
    bool has_data = false;
    for (size_t i = 0; i < input.size(); ++i) {
      has_data = true;
      output += input[i];
      output += ", ";
    }
    if (has_data) {
      output = output.substr(0, output.length() - 2);
    }
    return output;
  }
  const string name_;
  const vector<string> column_names_;
  const vector<vector<string> > data_; // only support string type for simplicity
};


class Database {
 public:
  Database(map<string, const Table*> table_map) : table_map_(table_map) {}
  ~Database() {
    for(map<string, const Table*>::iterator iterator = table_map_.begin(); iterator != table_map_.end(); ++iterator) {
      delete iterator->second;
    }
  }

  const Table* GetTable(const string& table_name) { return table_map_[table_name]; }

  // Callee will own the returned pointer
  Table* InnerJoin(
      const Table* left_table, const string& left_table_key_name,
      const Table* right_table, const string& right_table_key_name) 
  {
    const int column_count = 
      left_table->column_names().size() + right_table->column_names().size();

    vector<string> column_names(column_count);
    vector<vector<string>> data;

    string left_name = left_table->name() + '.';
    string right_name = right_table->name() + '.';

    int k = 0;
    const vector<string>& left_column_names = left_table->column_names();
    const vector<string>& right_column_names = right_table->column_names();
    int left_index = distance(left_column_names.begin(), 
        find(left_column_names.begin(), left_column_names.end(), left_table_key_name));
    int right_index = distance(right_column_names.begin(), 
        find(right_column_names.begin(), right_column_names.end(), right_table_key_name));

    for(const string& name : left_table->column_names())
    {
      column_names[k++] = left_name + name;
    }
    for(const string& name : right_table->column_names())
    {
      column_names[k++] = right_name + name;
    }

    vector<string> temp;

    for(const vector<string>& left : left_table->data())
    {
      
      for(const vector<string>& right : right_table->data())
      {
        if(left[left_index] != right[right_index]) continue;
      
        temp.insert(temp.begin(), left.begin(), left.end());
        temp.insert(temp.end(), right.begin(), right.end());
        data.push_back(temp);
        temp.clear();
      }
    }

    return new Table("InnerJoin", column_names, data);
  }

  // Callee will own the returned pointer
  Table* LeftJoin(
      const Table* left_table, const string& left_table_key_name,
      const Table* right_table, const string& right_table_key_name) 
  {
      vector<string> column_names;
      vector<vector<string>> data;
  
      string left_name = left_table->name() + '.';
      string right_name = right_table->name() + '.';
  
      const vector<string>& left_column_names = left_table->column_names();
      const vector<string>& right_column_names = right_table->column_names();
      int left_index = distance(left_column_names.begin(), 
          find(left_column_names.begin(), left_column_names.end(), left_table_key_name));
      int right_index = distance(right_column_names.begin(), 
          find(right_column_names.begin(), right_column_names.end(), right_table_key_name));
  
      for(const string& name : left_table->column_names())
      {
        column_names.push_back(left_name + name);
      }
      for(const string& name : right_table->column_names())
      {
        column_names.push_back(right_name + name);
      }
  
      vector<string> temp;
      bool flag = false;
  
      for(const vector<string>& left : left_table->data())
      {
        flag = false;
        
        for(const vector<string>& right : right_table->data())
        {
          if(left[left_index] != right[right_index]) continue;

          flag = true;
        
          temp.insert(temp.begin(), left.begin(), left.end());
          temp.insert(temp.end(), right.begin(), right.end());
          data.push_back(temp);
          temp.clear();
        }

        if(!flag)
        {
          temp = vector<string>(right_table->column_names().size());
          temp.insert(temp.begin(), left.begin(), left.end());
          data.push_back(temp);
          temp.clear();
        }
      }
  
      return new Table("LeftJoin", column_names, data);
  }
  Table* RightJoin(
      const Table* left_table, const string& left_table_key_name,
      const Table* right_table, const string& right_table_key_name) 
  {
      const int column_count = 
        left_table->column_names().size() + right_table->column_names().size();
  
      vector<string> column_names(column_count);
      vector<vector<string>> data;
  
      string left_name = left_table->name() + '.';
      string right_name = right_table->name() + '.';
  
      int k = 0;
      const vector<string>& left_column_names = left_table->column_names();
      const vector<string>& right_column_names = right_table->column_names();
      int left_index = distance(left_column_names.begin(), 
          find(left_column_names.begin(), left_column_names.end(), left_table_key_name));
      int right_index = distance(right_column_names.begin(), 
          find(right_column_names.begin(), right_column_names.end(), right_table_key_name));
  
      for(const string& name : left_table->column_names())
      {
        column_names[k++] = left_name + name;
      }
      for(const string& name : right_table->column_names())
      {
        column_names[k++] = right_name + name;
      }
  
      vector<string> temp;
      bool flag = false;
  
      for(const vector<string>& right : right_table->data())
      {
        flag = false;
        
        for(const vector<string>& left : left_table->data())
        {
          if(left[left_index] != right[right_index]) continue;

          flag = true;
        
          temp.insert(temp.begin(), left.begin(), left.end());
          temp.insert(temp.end(), right.begin(), right.end());
          data.push_back(temp);
          temp.clear();
        }

        if(!flag)
        {
          temp = vector<string>(left_table->column_names().size());
          temp.insert(temp.end(), right.begin(), right.end());
          data.push_back(temp);
          temp.clear();
        }
      }
  
      return new Table("Right Join", column_names, data);
  }

  // Callee will own the returned pointer
  Table* OuterJoin(
      const Table* left_table, const string& left_table_key_name,
      const Table* right_table, const string& right_table_key_name) 
  {

    Table* left_join = LeftJoin(left_table, left_table_key_name, right_table, right_table_key_name);
    Table* right_join = RightJoin(left_table, left_table_key_name, right_table, right_table_key_name);
    vector<vector<string>> data;
    vector<string> column_names = left_join->column_names();

    set_union(left_join->data().begin(), left_join->data().end(), right_join->data().begin(), right_join->data().end(), back_inserter(data));

    return new Table("OuterJoin", column_names, data); 

  }

 private:
  map<string, const Table*> table_map_;
};

#ifndef __main__
#define __main__

int main() {
  const Table* department_table = new Table(
      "departments",
      { "id", "name" },
      {
        { "0", "engineering" },
        { "1", "finance" }
      });

  const Table* user_table = new Table(
      "users",
      { "id", "department_id", "name" },
      {
        { "0", "0", "Ian" },
        { "1", "0", "John" },
        { "2", "1", "Eddie" },
        { "3", "1", "Mark" },
      });

  const Table* salary_table = new Table(
      "salaries",
      { "id", "user_id", "amount" },
      {
         { "0", "0", "100" },
         { "1", "1", "150" },
         { "2", "1", "200" },
         { "3", "3", "200" },
         { "4", "3", "300" },
         { "5", "4", "400" },
      });

  map<string, const Table*> table_map = map<string, const Table*>();
  table_map["departments"] = department_table;
  table_map["users"] = user_table;
  table_map["salaries"] = salary_table;
  Database* db = new Database(table_map);

  // should print
  // id, department_id, name
  // 1, 0, John
  {
    Table* filtered_table = db->GetTable("users")->Where("id", "1");
    Table* projected_table = filtered_table->Select({ "id", "department_id", "name" });
    projected_table->Print();
    delete projected_table;
    delete filtered_table;
  }

  // should print
  // users.name, departments.name
  // Ian, engineering
  // John, engineering
  {
    Table* table = db->InnerJoin(
        db->GetTable("users"),
        "department_id",
        db->GetTable("departments"),
        "id");
    Table* filtered_table = table->Where("departments.name", "engineering");
    Table* projected_table = filtered_table->Select({ "users.name", "departments.name" });
    projected_table->Print();
    delete projected_table;
    delete filtered_table;
    delete table;
  }

  // should print
  // users.name, salaries.amount
  // Ian, 100
  // John, 150
  // John, 200
  // Mark, 200
  // Mark, 300
  // Eddie,
  {
    Table* table = db->LeftJoin(
        db->GetTable("users"),
        "id",
        db->GetTable("salaries"),
        "user_id");
    Table* projected_table = table->Select({ "users.name", "salaries.amount" });
    projected_table->Print();
    delete projected_table;
    delete table;
  }

  // should print
  // users.name, salaries.amount
  // Ian, 100
  // John, 150
  // John, 200
  // Mark, 200
  // Mark, 300
  // , 400
  {
    Table* table = db->RightJoin(
        db->GetTable("users"),
        "id",
        db->GetTable("salaries"),
        "user_id");
    Table* projected_table = table->Select({ "users.name", "salaries.amount" });
    projected_table->Print();
    delete projected_table;
    delete table;
  }

  // should print
  // users.name, salaries.amount
  // Ian, 100
  // John, 150
  // John, 200
  // Mark, 200
  // Mark, 300
  // Eddie,
  // , 400
  {
    Table* table = db->OuterJoin(
        db->GetTable("users"),
        "id",
        db->GetTable("salaries"),
        "user_id");
    Table* projected_table = table->Select({ "users.name", "salaries.amount" });
    projected_table->Print();
    delete projected_table;
    delete table;
  }

  delete db;
}

#endif
