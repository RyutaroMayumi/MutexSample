#include <stdio.h>
#include <stdbool.h>

//#define USE_STD11
//#define USE_WIN32
//#define USE_POSIX

// standard c++11版
#ifdef USE_STD11
#include <thread>
#include <mutex>
#include <condition_variable>
std::mutex cond_var_mutex;			// 条件変数のミューテックス
std::condition_variable cond_var;	// スレッド待機制御用の条件変数

void wait_for_cond(std::function< bool() > f_get_cond)
{
	printf("Branch Thread: Start waiting.\n");
	{
		std::unique_lock<std::mutex> WFC_LOCK__COUNTER__(cond_var_mutex);	// ロックを取得する（std::condition_variable::wait()の事前条件, condition_variableと組み合わせ使えるのはunique_lockのみ）
		cond_var.wait(WFC_LOCK__COUNTER__, f_get_cond);						// 条件が満たされるまで待機する
		//WFC_LOCK__COUNTER__.unlock();										// wait()終了時に再ロックされるため、ロックを解除する。（unique_lockのデストラクタでロック解除されるためなくてもよい）
		//cond_var.notify_all();											// なくてもよい
	}
	printf("Branch Thread: Finish waiting.\n");
}

void set_cond(std::function< void() > f_set_cond)
{
	{
		std::lock_guard<std::mutex> SCO_LOCK__COUNTER__(cond_var_mutex);	// （cond_varの）ロックを取得
		f_set_cond();														// 条件変数を更新する
		cond_var.notify_all();												// 待機中のスレッドを起床させる
	}
	// std::lock_guardを使って取得したロックは、ロックオブジェクト破棄（コードブロック終了）時に自動的にロック解除される。
}
#endif

// WINAPI版
#ifdef USE_WIN32
#include <windows.h>
#include <process.h>
LPCTSTR mtxName = L"MUTEX";	// ミューテックスオブジェクト名

void wait_for_cond(void *p_cond)
{
	printf("Branch Thread: Start waiting.\n");
	{
		HANDLE h = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mtxName);	// ロックを取得する
		while (!(*(bool*)p_cond))								// 条件が満たされるまで待機する
		{
			WaitForSingleObject(h, 10);							// 10msの間ロックを解除したのち条件変数を確認する
		}
		CloseHandle(h);											// ロックを解除する
	}
	printf("Branch Thread: Finish waiting.\n");
}

void set_cond(bool *p_cond, bool cond)
{
	{
		HANDLE h = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mtxName);	// ロックを取得する
		*p_cond = cond;											// 条件変数を更新する
		CloseHandle(h);											// ロックを解除する
	}
}
#endif

// POSIX版
#ifdef USE_POSIX
#include <pthread.h>
#include <unistd.h>
pthread_mutex_t cond_var_mutex;	// 条件変数のミューテックス
pthread_cond_t cond_var;		// スレッド待機制御用の条件変数

void* wait_for_cond(void *p_cond)
{
	printf("Branch Thread: Start waiting.\n");
	{
		pthread_mutex_lock(&cond_var_mutex);			// ロックを取得する
		pthread_cond_wait(&cond_var, &cond_var_mutex);	// 条件が満たされるまで待機する
		pthread_mutex_unlock(&cond_var_mutex);			// ロックを解除する
	}
	printf("Branch Thread: Finish waiting.\n");
}

void set_cond(bool *p_cond, bool cond)
{
	{
		pthread_mutex_lock(&cond_var_mutex);			// ロックを取得する
		*p_cond = cond;									// 条件変数を更新する
		pthread_mutex_unlock(&cond_var_mutex);			// ロックを解除する
		pthread_cond_signal(&cond_var);					// 待機中のスレッドを起床させる
	}
}
#endif


int main(int argc, char **argv)
{
	bool cond = false;

	printf("Trunk Thread: Begin a thread for waiting the condition changed.\n");
#ifdef USE_STD11
	std::thread th(wait_for_cond, [&] { return cond; });	// ラムダ式のサポートはc++11から（[&]でローカル変数の参照キャプチャ）
#endif
#ifdef USE_WIN32
	HANDLE mtx = (HANDLE)CreateMutex(NULL, FALSE, mtxName);
	HANDLE th = (HANDLE)_beginthread(wait_for_cond, 0, &cond);
#endif
#ifdef USE_POSIX
	pthread_mutex_init(&cond_var_mutex, NULL);
	pthread_cond_init(&cond_var, NULL);
	pthread_t th;
	pthread_create(&th, NULL, wait_for_cond, &cond);
#endif

	printf("Trunk Thread: Sleep main thread for 10 seconds to change the condition.\n");
#ifdef USE_STD11
	std::this_thread::sleep_for(std::chrono::seconds(10));
	set_cond([&] { cond = true; });
#endif
#ifdef USE_WIN32
	Sleep(10 * 1000);
	set_cond(&cond, true);
#endif
#ifdef USE_POSIX
	sleep(10);
	set_cond(&cond, true);
#endif

	printf("Trunk Thread: Wait branch thread to finish.\n");
#ifdef USE_STD11
	th.join();
#endif
#ifdef USE_WIN32
	WaitForSingleObject(th, INFINITE);
	ReleaseMutex(mtx);
#endif
#ifdef USE_POSIX
	pthread_join(th, NULL);
#endif

	return 0;
}


