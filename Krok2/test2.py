from PIL import Image
import os
import threading

nthreads = 2
global columns
global rows
global res_dir_path
global files

def scan_to_pieces(name, piece_numbers):
    scan = Image.open(name)
    pix = scan.load()
    scan_width, scan_height = scan.size
    piece_width = scan_width // columns
    piece_height = scan_height // rows
    for r in range(rows):
        for c in range(columns):
            piece_file_name = "p" + str(piece_numbers + columns * r + c)
            piece_file_path = os.path.join(res_dir_path, piece_file_name)
            piece_file = open(piece_file_path, "w")
            piece_file.write(str(piece_width) + " " + str(piece_height) + "\n")
            corner_x = piece_width * c
            corner_y = piece_height * r
            for y in range(corner_y, corner_y+piece_height):
                for x in range(corner_x, corner_x + piece_width):
                    pixel = pix[x, y]
                    piece_file.write(str(pixel[0]) + " " + str(pixel[1]) + " " + str(pixel[2]) + "\n")
            piece_file.close()
    scan.close()
    print("Skan gotowy")

def start_thread(v):
    n = len(files)
    for i in range(v, n, nthreads):
        scan_to_pieces(files[i], 35*i)

puzzle_pieces = int(input('Z ilu elementów składają się Twoje puzzle: '))
columns = int(input('Ile kolumn puzzli mieści się na jednym skanie: '))
rows = int(input('A ile wierszy: '))
dir = input('Podaj nazwę folderu, w którym znajdują się skany puzzli: ')
res_dir = input('Podaj nazwę folderu, w którym zostaną zapisane wyniki działania programu: ')

dir_path = os.path.join(os.getcwd(), dir)
files = [os.path.join(dir_path, f) for f in os.listdir(dir_path)]

res_dir_path = os.path.join(os.getcwd(), res_dir)
os.mkdir(res_dir_path)

print("Postęp:")

thread_list = []
for i in range(nthreads):
    t = threading.Thread(target=start_thread, args=(i, ))
    thread_list.append(t)

for t in thread_list:
    t.start()

for t in thread_list:
    t.join()
   