#include <mutex>

// PSEUDO CODE!!!

std::mutex mtx1;
std::mutex mtx2;

int main() {
	std::lock(mtx1, mtx2);

	std::lock_guard<std::mutex> l1(mtx1, std::adopt_lock);
	std::lock_guard<std::mutex> l2(mtx2, std::adpot_lock);

	// Transfer ....
	
}
