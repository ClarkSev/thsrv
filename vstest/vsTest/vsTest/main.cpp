/*
	测试文件
*/

#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

class Buffer
{
public:
	// ctor. ---- copy and move ctor. use default function.
	explicit Buffer(int initSz = 1024) :shReadbytes_(0), shWritebytes_(0),
		bufReadbytes_(0), bufWritebytes_(0)
	{
		rdbuf_.resize(initSz);
		wtbuf_.resize(initSz);
	}
	void swap(Buffer& others)
	{
		std::swap(shReadbytes_, others.shReadbytes_);
		std::swap(shWritebytes_, others.shWritebytes_);
		std::swap(bufReadbytes_, others.bufReadbytes_);
		std::swap(bufWritebytes_, others.bufWritebytes_);
		std::swap(rdbuf_, others.rdbuf_);
		std::swap(wtbuf_, others.wtbuf_);
	}

	size_t readable() const { return bufReadbytes_ - shReadbytes_; }
	size_t writable() const { return bufWritebytes_ - shWritebytes_; }
	size_t appendable_rdbuf() const { return rdbuf_.size() - bufReadbytes_; }
	size_t appendable_wtbuf() const { return wtbuf_.size() - bufWritebytes_; }

	void showElem()const
	{
		cout << "wtbuf_:" << endl;
		for (auto elem : wtbuf_) {
			cout << "elem = " << elem << "  ";
		}
		cout << "rdbuf_:" << endl;
		for (auto elem : rdbuf_) {
			cout << "elem = " << elem << "  ";
		}
	}
	void clear() { wtbuf_.clear(); rdbuf_.clear(); }
	void appendInWtBuf(const char* buf, size_t tlen)
	{
		if (tlen >= appendable_wtbuf()) {
			makeSpaceWtBuf();
		}
		wtbuf_.assign(buf, buf + tlen);
	}
	void appendInRdBuf(const char* buf, size_t tlen)
	{
		if (tlen >= appendable_rdbuf()) {
			makeSpaceRdBuf();
		}
		rdbuf_.assign(buf, buf + tlen);
	}

	char* beginWriteInBuf()
	{
		return &*wtbuf_.begin() + bufWritebytes_;
	}
	char* beginReadInBuf()
	{
		return &*rdbuf_.begin() + bufReadbytes_;
	}
	char* beginWriteOutBuf()
	{
		return &*wtbuf_.begin() + shWritebytes_;
	}
	char* beginReadOutBuf()
	{
		return &*rdbuf_.begin() + shReadbytes_;
	}

private:  // private method
	void makeSpaceRdBuf()
	{
		size_t lszbuf = rdbuf_.size();
		rdbuf_.resize(lszbuf * 2);
	}
	void makeSpaceWtBuf()
	{
		size_t lszbuf = wtbuf_.size();
		wtbuf_.resize(lszbuf * 2);
	}
private:
	// 终端已读数据
	size_t shReadbytes_;
	// 终端已提取数据
	size_t shWritebytes_;
	// buffer
	size_t bufReadbytes_;
	size_t bufWritebytes_;

	std::vector<char>rdbuf_;
	std::vector<char>wtbuf_;

};

int main()
{
	Buffer buf1;
	string str1 = "12345678";
	string str2 = "abcdefghijklmn";
	buf1.appendInWtBuf(str1.data(), str1.size());
	buf1.appendInRdBuf(str2.data(), str2.size());
	cout << "buf1----->" << endl;
	buf1.showElem();
	Buffer buf2 = buf1;
	cout << "buf2----->" << endl;
	buf2.showElem();
	buf1.clear();
	cout << "buf1----->" << endl;
	buf1.showElem();
	cout << "buf2----->" << endl;
	buf2.showElem();
	system("pause");
	return 0;
}



