from PIL import Image
from subprocess import call
import os
import threading

nthreads = 4
global puzzle_pieces
global columns
global rows
global res_dir_path
global files

def scan_to_pieces(name, scan_number):
    scan = Image.open(name)
    pix = scan.load()
    scan_width, scan_height = scan.size
    piece_width = scan_width // columns
    piece_height = scan_height // rows
    piece_numbers = scan_number*rows*columns
    for r in range(rows):
        for c in range(columns):
            if(piece_numbers + columns * r + c >= puzzle_pieces):
                break
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
    print("Skan", scan_number + 1, "gotowy")

def start_thread(v):
    n = len(files)
    for i in range(v, n, nthreads):
        scan_to_pieces(files[i], i)

def sliceScans():
    global puzzle_pieces
    puzzle_pieces = int(input('Z ilu elementów składają się Twoje puzzle: '))
    global columns
    columns = int(input('Ile kolumn puzzli mieści się na jednym skanie: '))
    global rows
    rows = int(input('A ile wierszy: '))
    dir = input('Podaj nazwę folderu, w którym znajdują się skany puzzli: ')

    dir_path = os.path.join(os.getcwd(), dir)
    global files
    files = [os.path.join(dir_path, f) for f in sorted(os.listdir(dir_path))]

    global res_dir_path
    res_dir_path = os.path.join(os.getcwd(), "Puzzles")
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

    print("Pomyślnie ukończono przetwarzanie skanów\n")
    return puzzle_pieces
    