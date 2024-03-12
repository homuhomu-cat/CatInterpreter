# pragma once
# include <iostream>
# include <vector>
# include <fstream>
# include <unordered_map>
# include <algorithm>

namespace cat_lib 
{
	//Catクラス
	class Cat 
	{
	private:
		static constexpr auto Npos = std::string::npos;

		//スクリプト
		std::vector<std::vector<std::string>> script;

		inline size_t find(const std::vector<std::string>& arr, std::string target) const noexcept
		{
			for (size_t i = 0; i < arr.size(); ++i)
			{
				if (arr.at(i) == target)
					return i;
			}

			return Npos;
		}

		inline std::vector<std::string> substr(const std::vector<std::string>& arr, size_t pos, size_t length) const
		{
			std::vector<std::string> result;

			for (size_t i = pos; (length == Npos) ? (i < arr.size()) : (i < pos + length); ++i)
			{
				result.emplace_back(arr.at(i));
			}

			return result;
		}

		inline std::vector<std::string> concat(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) const
		{
			std::vector<std::string> result = lhs;
			result.insert(result.end(), rhs.begin(), rhs.end());

			return result;
		}

		inline std::vector<std::string> split(std::string str, std::string arg) const
		{
			std::vector<std::string> arr;
			
			size_t pos;
			while ((pos = str.find(arg)) != std::string::npos)
			{
				arr.emplace_back(str.substr(0, pos));
				str = str.substr(pos + arg.length(), std::string::npos);
			}
			arr.emplace_back(str);

			return arr;
		}

	private:
		//動作
		std::string execute(size_t index, std::vector<std::string> line, std::vector<std::string> args) const
		{
			std::unordered_map<std::string, std::string> args_map;

			std::vector<std::string> left;
			std::vector<std::string> right;

			{
				auto pos = this->find(line, "->");
				if (pos != Npos)
				{
					left = this->substr(line, 0, pos);
					right = this->substr(line, pos + 1, Npos);

					for (size_t i = 0; i < left.size(); ++i)
					{
						auto& key = left.at(i);
						args_map[key] = args.at(i);
					}

					for (auto& p : right)
					{
						if (args_map.count(p) != 0)
							p = args_map.at(p);
					}
				}
				else
				{
					right = line;
				}
			}

			//三項演算子
			{
				const size_t pos_q = this->find(right, "?");
				const size_t pos_c = this->find(right, ":");

				if (pos_q != Npos && pos_c != Npos)
				{
					std::vector<std::string> left_p = left;
					left_p.emplace_back("->");
					const std::vector<std::string> cond = this->concat(left_p, this->substr(right, 0, pos_q));

					const std::string result = this->execute(index, cond, args);

					if (std::stoi(result) != 0)
					{
						const std::vector<std::string> right_p = this->concat(left_p, this->substr(right, pos_q + 1, pos_c - pos_q - 1));

						const std::string result_p = this->execute(index, right_p, args);
						return result_p;
					}
					else
					{
						const std::vector<std::string> right_p = this->concat(left_p, this->substr(right, pos_c + 1, Npos));

						const std::string result_p = this->execute(index, right_p, args);
						return result_p;
					}
				}
			}

			//処理
			for (int pos = right.size() - 1; pos >= 0; --pos)
			{
				const auto& p = right.at(pos);

				//呼出
				if(p == "<<" && pos >= 1)
				{
					const std::string obj = right.at(pos - 1);
					const auto arg = this->substr(right, pos + 1, Npos);

					if (obj == "^")
					{
						const std::string result = this->execute(index - 1, script.at(index - 1), arg);

						right.erase(right.begin() + (pos - 1), right.end());
						right.emplace_back(result);

						--pos;
					}
					else if (obj == ".")
					{
						const std::string result = this->execute(index, script.at(index), arg);

						right.erase(right.begin() + (pos - 1), right.end());
						right.emplace_back(result);

						--pos;
					}
					else if (obj == "~")
					{
						auto arg_r = arg;
						std::reverse(arg_r.begin(), arg_r.end());

						right.erase(right.begin() + (pos - 1), right.end());
						right.insert(right.end(), arg_r.begin(), arg_r.end());

						--pos;
					}
					else if (obj == "+" && arg.size() >= 2)
					{
						const std::string result = std::to_string(std::stoi(arg.at(0)) + std::stoi(arg.at(1)));

						right.erase(right.begin() + (pos - 1), right.begin() + pos + 3);
						right.insert(right.begin() + (pos - 1), result);

						--pos;
					}
					else if (obj == "-" && arg.size() >= 2)
					{
						const std::string result = std::to_string(std::stoi(arg.at(0)) - std::stoi(arg.at(1)));

						right.erase(right.begin() + (pos - 1), right.begin() + pos + 3);
						right.insert(right.begin() + (pos - 1), result);

						--pos;
					}
					else if (obj == ">" && arg.size() >= 2)
					{
						const std::string result = std::stoi(arg.at(0)) > std::stoi(arg.at(1)) ? "1" : "0";

						right.erase(right.begin() + (pos - 1), right.begin() + pos + 3);
						right.insert(right.begin() + (pos - 1), result);

						--pos;
					}
					else if (obj == "!" && arg.size() >= 1)
					{
						const std::string result = std::stoi(arg.at(0)) == 0 ? "1" : "0";

						right.erase(right.begin() + (pos - 1), right.begin() + pos + 2);
						right.insert(right.begin() + (pos - 1), result);

						--pos;
					}
				}
				//入力
				else if (p == "<-" && pos >= 1)
				{
					const auto arg = this->substr(right, pos + 1, Npos);

					std::string str;
					std::cin >> str;

					right.erase(right.begin() + (pos - 1), right.begin() + pos + 1);
					right.insert(right.begin() + (pos - 1), str);

					--pos;
				}
				//出力
				else if (p == "<-" && pos == 0 && right.size() >= 2)
				{
					const std::string str = right.at(pos + 1);
					std::cout << str << std::endl;

					right.erase(right.begin() + pos, right.end());
					right.emplace_back(str);
				}
			}
			
			if (right.size() == 1)
				return right.front();
			else
				return "";
		}

		//動作
		void init() const
		{
			for (size_t i = 0; i < script.size(); ++i)
			{
				const auto& line = script.at(i);
				if (this->find(line, "->") == Npos)
				{
					execute(i, line, std::vector<std::string>());
				}
			}
		}

	public:
		Cat(std::ifstream& file)
		{
			std::string line;

			//ファイルを読み込む
			while (std::getline(file, line)) 
			{ 
				std::cout << line << std::endl;

				//行ごとに保存
				auto a = this->split(line, " ");
				script.emplace_back(a);
			}

			std::cout << std::endl;

			this->init();
		}
	};
}