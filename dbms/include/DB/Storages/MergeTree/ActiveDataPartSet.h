#pragma once
#include <Poco/Mutex.h>
#include <Poco/RegularExpression.h>
#include <Yandex/DateLUT.h>
#include <DB/Core/Types.h>
#include <set>

namespace DB
{

/** Поддерживает множество названий активных кусков данных.
  * Повторяет часть функциональности MergeTreeData.
  * TODO: обобщить с MergeTreeData. Можно этот класс оставить примерно как есть и использовать его из MergeTreeData.
  *       Тогда в MergeTreeData можно сделать map<String, DataPartPtr> data_parts и all_data_parts.
  */
class ActiveDataPartSet
{
public:
	struct Part
	{
		DayNum_t left_date;
		DayNum_t right_date;
		UInt64 left;
		UInt64 right;
		UInt32 level;
		std::string name;
		DayNum_t left_month;
		DayNum_t right_month;

		bool operator<(const Part & rhs) const
		{
			if (left_month != rhs.left_month)
				return left_month < rhs.left_month;
			if (right_month != rhs.right_month)
				return right_month < rhs.right_month;

			if (left != rhs.left)
				return left < rhs.left;
			if (right != rhs.right)
				return right < rhs.right;

			if (level != rhs.level)
				return level < rhs.level;

			return false;
		}

		/// Содержит другой кусок (получен после объединения другого куска с каким-то ещё)
		bool contains(const Part & rhs) const
		{
			return left_month == rhs.left_month		/// Куски за разные месяцы не объединяются
				&& right_month == rhs.right_month
				&& level > rhs.level
				&& left_date <= rhs.left_date
				&& right_date >= rhs.right_date
				&& left <= rhs.left
				&& right >= rhs.right;
		}
	};

	ActiveDataPartSet();

	void add(const String & name);
	String getContainingPart(const String & name);

	static String getPartName(DayNum_t left_date, DayNum_t right_date, UInt64 left_id, UInt64 right_id, UInt64 level);

	/// Возвращает true если имя директории совпадает с форматом имени директории кусочков
	static bool isPartDirectory(const String & dir_name, Poco::RegularExpression::MatchVec & matches);

	/// Кладет в DataPart данные из имени кусочка.
	static void parsePartName(const String & file_name, Part & part, const Poco::RegularExpression::MatchVec * matches = nullptr);

private:
	typedef std::set<Part> Parts;

	Poco::Mutex mutex;
	Parts parts;
};

}