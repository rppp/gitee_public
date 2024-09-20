import subprocess
from multiprocessing import Pool, cpu_count
import time

def single_ping(host="8.8.8.8"):
    ping_command = ["ping", "-c", "1", "-W", "1", host]
    try:
        subprocess.check_output(ping_command, stderr=subprocess.STDOUT, timeout=1)
        return 0  # 成功
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
        return 1  # 失败或超时

def ping_test(host="8.8.8.8", count=100):
    pool = Pool(processes=min(cpu_count(), 30))  # 限制最大进程数为30
    results = pool.map(single_ping, [host] * count)
    pool.close()
    pool.join()
    return sum(results)
    #packet_loss = (sum(results) / count) * 100
    #return packet_loss

def main():
    print("开始网络测试...")
    
    host = input("请输入要ping的主机（默认为8.8.8.8）: ") or "www.hao123.com"
    count = int(input("请输入ping的次数（默认为100）: ") or "100")
    
    print(f"\n测试丢包率... 主机: {host}, 次数: {count}")
    start_time = time.time()
    packet_loss = ping_test(host, count)
    end_time = time.time()
    packet_loss_p = (packet_loss / count) * 100
    
    print("丢包"+str(packet_loss))
    print(f"丢包率: {packet_loss_p:.2f}%")
    print(f"ping测试耗时: {end_time - start_time:.2f} 秒")

if __name__ == "__main__":
    main()