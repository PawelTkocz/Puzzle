from PIL import Image
import os


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

scans_done = 0
pieces_per_scan = rows * columns
for f in files:
    scan = Image.open(f)
    pix = scan.load()
    scan_width, scan_height = scan.size
    piece_width = scan_width // columns
    piece_height = scan_height // rows
    for r in range(rows):
        for c in range(columns):
            piece_file_name = "p" + str(pieces_per_scan*scans_done + columns * r + c)
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
    print("Skan gotowy")
    scans_done += 1

#im = Image.open('wiewiorka.jpg')
#pix = im.load()
#print (im.size)
#print(pix[0, 0])
#pix[0, 0] = (0, 0, 0)
#im.save('wiewiorka2.png')