string readfile(const string filename)
{
	ifstream fin(filename, ios::binary);
	string bodem = "";
	if (!fin.is_open()) 
	{
		cout << "Cannot open this file: " << filename << "\n";
		return "";
	}
	stringstream buffer;
	buffer << fin.rdbuf();
	fin.close();
	return buffer.str();
}
bool writefile(const string filename, const string code)
{
	ofstream fout(filename, ios::binary);
	if (!fout.is_open())
	{
		cout << "Cannot open this file: " << "\n";
		return false;
	}
	fout << code;
	fout.close();
	return true;
}
int checkinDictionary(const vector<string>& dictionary, string bodem)
{
	int left = 0;
	int right = dictionary.size() - 1;
	if (bodem.length() == 1)
	{
		while (left <= right)
		{
			int mid = (left + right) / 2;
			if (dictionary[mid] == bodem)
				return mid;
			else if (dictionary[mid] < bodem)
				left = mid + 1;
			else
				right = mid - 1;
		}
	}
	else
	{
		for (int i = 256; i < dictionary.size(); i++)
		{
			if (dictionary[i] == bodem)
				return i;
		}
	}
	return -1;
}
vector<string> initDictionary()
{
	vector<string> dictionary(256);
	for (int i = 0; i < 256; i++)
	{
		dictionary[i] = (char)i;
	}
	return dictionary;
}
string compress(const string& text) // nen du lieu 
{
	if (text.empty()) return "";
	string w = "";
	vector<int> res;
	vector<string>dictionary = initDictionary();
	for (int i = 0; i < text.length(); i++)
	{
		char kytu = text[i];
		string nw = w + kytu;
		if (checkinDictionary(dictionary, nw)!= -1)
			w = nw;
		else
		{
			dictionary.push_back(nw);
			res.push_back(checkinDictionary(dictionary, w));
			w = nw[nw.length() - 1];
		}

	}
	string output = "";
	for (int i = 0; i < res.size(); i++)
	{
		output += to_string(res[i]) + " ";
	}
	return output;
}
string decompress(const string& num) // giai nen du lieu 
{
	stringstream ss(num);
	vector<int>res;
	vector<string>dictionary = initDictionary();
	string temp = "";
	// doc ma
	while (getline(ss, temp, ' '))
	{
		res.push_back(stoi(temp));
	}
	// doi lai thanh chu
	temp = "";
	int old = res[0];
	string w = dictionary[old];
	string result = w;
	for (int i = 1; i < res.size(); i++)
	{
		int id = res[i];
		if (id < dictionary.size())
		{
			temp = dictionary[id];
		}
		else if (id == dictionary.size())
		{
			temp = w + w[0];
		}
		result += temp;
		dictionary.push_back(w + temp[0]);
		w = temp;
	}
	return result;
}
long long getFileSize(const string& filename) {
	ifstream file(filename, ios::binary | ios::ate);
	if (!file.is_open()) return 0;
	return file.tellg(); 
}
void output(string algo, long long initial, long long after, double ratio, double ssaving)
{
	cout << "Compression complete." << "\n";
	cout << "-------------------------------" << "\n";
	cout << "Algorithm: " <<algo << "\n";
	cout << "Original size: " << initial << "\n";
	cout << "Compressed size: " << after << "\n";
	cout << "Compression ratio: " << ratio << "\n";
	cout << "Space saving: " << ssaving << "\n";
}
double calcRatio(long long initial, long long after)
{
	return (initial * 1.0 / after);
}
double calcSpaceS(long long initial, long long after)
{
	return 1 - (after * 1.0 / initial);
}
