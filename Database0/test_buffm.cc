#include "buffer-manager.h"
#include <iostream>

int main()
{
	BufferManager bm;
	const char* db_file_name = "./test.db";
	FileId db_file;
	std::vector<std::string> test;
	test.push_back("This is test A .....");
	test.push_back("db_file = bm.NewFile(db_file_name);");
	test.push_back("auto v = bm.GetPage<char*>(db_file, column_Var);");
	test.push_back("for (int i = 0; i < 10; ++i) {");
	test.push_back("++itr;");
	test.push_back("itr.Insert(10.0 * i);");
	srand(0);
	constexpr int fill_up = Page::kDiscretionSpace / sizeof(double) + 100;
	// write data
	if (!fs::exists(db_file_name)) {
		db_file = bm.NewFile(db_file_name);

		// allocate first page for column A
		PageId column_A_1 = bm.AllocatePageAfter(db_file, 0);
		PageId column_Var = bm.AllocatePageAfter(db_file, 0);
		PageId column_A_2 = bm.AllocatePageAfter(db_file, column_A_1);
		// get an iterator on the page
		auto itr = bm.GetPage<double>(db_file, column_A_1);

		// char* 是一个特殊的迭代器，表示变长的数据，边长数据理论上可以存放任何类型的数据，但会占更多空间
		// auto v = bm.GetPage<char*>(db_file, column_Var);
		auto last_page = &itr.page();
		for (int i = 0; i < fill_up; ++i) {
			if (!itr.Insert(/* (double)rand() / RAND_MAX*/ double(i))) {
				++itr;
				itr.Insert(double(i));
			}
			
			if (&itr.page() != last_page) {
				std::cerr << "We've just walk into another page!!" << std::endl;
				last_page = &itr.page();
				getchar();
			}
			++itr;
		}

		//for (int i = 0; i < 10; ++i) {
		//	v.Insert(test[i % test.size()]);
		//	++v;
		//}
	}
	// read data
	else {
		db_file = bm.OpenFile(db_file_name);

		// 因为之前分配的column 的PageId 是 1,这里偷懒直接用1, 这应该由 Catalog Manager 记录
		// Catalog Manager 知道表中每一列对应的第一个PageId, 查询列的时候应该返回一个指向第一行的迭代器
		// Index Manager 应该记录哪些Key对应哪一个PageId, 查询Key的时候返回 一个迭代器
		
		//TODO 计划在 单个Page 上实现二分查找
		auto itr = bm.GetPage<double>(db_file, 1);
		auto v = bm.GetPage<char*>(db_file, 2);
		auto itr2 = bm.GetPage<double>(db_file, 3);
		for (int i = 0; i < fill_up; ++i) {
			
			if (*itr != double(i) /*(double)rand() / RAND_MAX*/) {
				std::cout << "row " << i << " fail" << std::endl;
			}
			++itr;
			if (itr.IsEndPage()) {
				++itr;
			}
		}
		
		getchar();
		getchar();
	}

	

}