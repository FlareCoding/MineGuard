import termcolor
from config_manager import config_reader
from network import command_client
from termcolor import colored
from socket import error as socket_error
import threading
import os
import time

cmd_client = None
config = None
g_apply_overclocks = False
g_all_device_stats = []

def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def show_welcome_screen():
    print(colored(
'''
#     _____  .__                ________                       .___
#    /     \ |__| ____   ____  /  _____/ __ _______ _______  __| _/
#   /  \ /  \|  |/    \_/ __ \/   \  ___|  |  \__  \\_  __ \/ __ | 
#  /    Y    \  |   |  \  ___/\    \_\  \  |  // __ \|  | \/ /_/ | 
#  \____|__  /__|___|  /\___  >\______  /____/(____  /__|  \____ | 
#          \/        \/     \/        \/           \/           \/ 
\n''', 'yellow'))

def connect_cmd_client():
    global cmd_client
    global config

    cmd_client = command_client(config)
    connected = False

    while not connected:
        print(colored('[*] Attempting to connect to rig server [{}:{}]'.format(cmd_client.get_server_ip(), str(cmd_client.get_server_port())), 'cyan'))
        connected = cmd_client.connect()
        if connected is True:
            break

        print(colored('[*] Failed to connect to rig server [{}:{}]'.format(cmd_client.get_server_ip(), str(cmd_client.get_server_port())), 'red'))

    print(colored('[*] Connected to rig server [{}:{}]\n'.format(cmd_client.get_server_ip(), str(cmd_client.get_server_port())), 'cyan'))

def show_keyboard_options():
    print(colored("Press 'x' + ENTER to restart MineGuard server (not available)", 'yellow'))
    print(colored("Press 'r' + ENTER to reboot the rig (not available)", 'yellow'))
    print(colored("Press 'o' + ENTER to reapply overclocks", 'yellow'))
    print()

def keyboard_listener():
    global cmd_client
    global g_apply_overclocks

    while True:
        key = input()
        if key == "r":
            # reboot rig
            pass
        if key == "s":
            # shutdown miner
            pass
        if key == "o":
            print('\033[F\033[A')
            print(colored('   [*] Applying overclock settings... ', 'cyan'))
            g_apply_overclocks = True
            pass
        
        buf = ''
        for _ in range(len(key)):
            buf += '-'

        print("\033[F\033[A")
        print('\r{}'.format(buf), end='\r')

def start_keyboard_listener():
    thread = threading.Thread(target=keyboard_listener)
    thread.daemon = True
    thread.start()

def init_all_device_stats(device_count):
    global g_all_device_stats

    for _ in range(device_count):
        stats = {}
        stats['max_temperature'] = 0
        g_all_device_stats.append(stats)

def get_gpu_device_count():
    global cmd_client
    
    cmd_client.send_cmd('mg_get_device_count')
    status, device_count = cmd_client.receive_response()

    if status == False:
        return 0

    return int(device_count)

def get_gpu_stats(device):
    global cmd_client
    global g_all_device_stats

    g_all_device_stats[device]['device_index'] = device

    cmd_client.send_cmd('mg_get_device_stats')
    _ = cmd_client.receive_response() # ping (request accepted)

    cmd_client.send_cmd(str(device))

    _, name = cmd_client.receive_response()
    cmd_client.send_cmd('response accepted')

    try:
        name = name[:name.index('\x00')]
    except:
        pass

    _, temperature = cmd_client.receive_response()
    cmd_client.send_cmd('response accepted')

    _, fan_speed   = cmd_client.receive_response()
    cmd_client.send_cmd('response accepted')

    _, power_usage = cmd_client.receive_response()
    cmd_client.send_cmd('response accepted')

    _, hashrate    = cmd_client.receive_response()

    g_all_device_stats[device]['name']           = name
    g_all_device_stats[device]['temperature']    = temperature
    g_all_device_stats[device]['fan_speed']      = fan_speed
    g_all_device_stats[device]['power_usage']    = power_usage
    g_all_device_stats[device]['algo']           = colored('ProgPow', 'green')
    
    if hashrate == 'share accepted':
        g_all_device_stats[device]['hashrate'] = 'share accepted'
    elif hashrate == 'inactive':
        g_all_device_stats[device]['hashrate'] = '0 MH/s'
        g_all_device_stats[device]['algo'] = colored('RandomX', 'yellow')
    else:
        g_all_device_stats[device]['hashrate'] = hashrate + ' MH/s'

    try:
        if int(temperature) > int(g_all_device_stats[device]['max_temperature']):
            g_all_device_stats[device]['max_temperature'] = temperature
    except:
        pass

    return g_all_device_stats[device]

def apply_overclocks():
    global cmd_client

    cmd_client.send_cmd('mg_set_device_overclocks')
    _ = cmd_client.receive_response() # confirmation

def show_gpu_device_list(gpu_device_count):
    stats = []
    for i in range(gpu_device_count):
        stats.append(get_gpu_stats(i))

    print(colored('GPU Devices Found:', 'yellow'))
    for i in range(gpu_device_count):
        print('[{}]:  {}'.format(i, colored(stats[i]['name'], 'green')))

    print()

def show_monitor_screen(gpu_device_count):
    global g_apply_overclocks

    stats = []
    for i in range(gpu_device_count):
        stats.append(get_gpu_stats(i))

    if stats[0]['power_usage'] == 0:
        print('\t'.expandtabs(50), end='\r')
        print(colored(' [*]', 'red') + ' Miner is ' + colored('offline', 'red'), end='\r')
        return

    # Check for flags raised by the keyboard listener
    if g_apply_overclocks == True:
        apply_overclocks()
        print(colored('   [*] Overclock settings applied!    ', 'green'), end='\r')
        time.sleep(1)
        g_apply_overclocks = False

    for gpu in stats:
        print('\t\t\t\t\t\t'.expandtabs(20).replace(' ', '-'))
        print(colored('         Temp.\tMax Temp.\tFan Speed\tPower Usage\tHashrate\tAlgo.'.expandtabs(20), 'cyan'))
        print('[GPU{}]   {}C\u00B0\t{}C\u00B0\t{}%\t{}W\t          \b\b\b\b\b\b\b\b\b\b{}\t{}\t'.format(gpu['device_index'], gpu['temperature'], gpu['max_temperature'], gpu['fan_speed'], gpu['power_usage'], gpu['hashrate'], gpu['algo']).expandtabs(20))

    print('\t\t\t\t\t\t'.expandtabs(20).replace(' ', '-'))

    for i in range(gpu_device_count):
        print("\033[F\033[A")
        print("\033[F\033[A")
        print("\033[F\033[A")

    print("\033[F\033[A")

def main():
    global cmd_client
    global config

    clear_screen()
    show_welcome_screen()
    show_keyboard_options()

    config = config_reader('config.json')

    connect_cmd_client()
    gpu_device_count = get_gpu_device_count()

    init_all_device_stats(gpu_device_count)

    show_gpu_device_list(gpu_device_count)
    start_keyboard_listener()

    try:
        while True:
            show_monitor_screen(gpu_device_count)
            time.sleep(config.get('monitor-update-interval'))
    except KeyboardInterrupt:
        print("\033[F\033[A")
        print('[*] Exiting...')
    except socket_error:
        print("\033[F\033[A")
        print(colored('[*] Lost connection to rig server [{}:{}]'.format(cmd_client.get_server_ip(), str(cmd_client.get_server_port())), 'red'))
        print('[*] Exiting...')

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('[*] Exiting...\n')
    except socket_error:
        print(colored('[*] Lost connection to rig server [{}:{}]'.format(cmd_client.get_server_ip(), str(cmd_client.get_server_port())), 'red'))
