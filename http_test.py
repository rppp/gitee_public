import requests
from multiprocessing import Pool, cpu_count
import time
import statistics

g_s = '''
<html>
<head><title>404 Not Found</title></head>
<body>
<center><h1>404 Not Found</h1></center>
<hr><center>nginx/1.27.1</center>
</body>
</html>
'''

def single_request(url):
    start_time = time.time()
    try:
        response = requests.get(url, timeout=4)
        end_time = time.time()
        delay = end_time - start_time
        if response.status_code == 404 and "<h1>404 Not Found</h1>" in response.text:
            return (0, delay) 
        else: 
            print('error1. delay ', delay)
            return (1, delay)
    except requests.RequestException:
        end_time = time.time()
        delay = end_time - start_time
        print('error2. delay ', delay)
        return (1, delay)  # 请求失败

def http_test(url, count):
    pool = Pool(processes=60)  # 限制最大进程数
    results = pool.map(single_request, [url] * count)
    pool.close()
    pool.join()
    return results

def main():
    print("开始网络测试...")
    
    url = "http://b.rppp.ren/" #发现用500次，5g郴州会丢包，但电信宽带不会
    count = 3000 #3000次大约21秒
    
    print(f"\n测试失败率和延迟... URL: {url}, 次数: {count}")
    start_time = time.time()
    results = http_test(url, count)
    end_time = time.time()
    
    failure_count = sum(result[0] for result in results)
    failure_rate = (failure_count / count) * 100
    
    delays = [result[1] for result in results]
    max_delay = max(delays)
    min_delay = min(delays)
    avg_delay = statistics.mean(delays)
    
    print(f"失败次数: {failure_count}")
    print(f"失败率: {failure_rate:.2f}%")
    print(f"最大延迟: {max_delay:.3f} 秒")
    print(f"最小延迟: {min_delay:.3f} 秒")
    print(f"平均延迟: {avg_delay:.3f} 秒")
    print(f"HTTP测试总耗时: {end_time - start_time:.2f} 秒")

if __name__ == "__main__":
    main()