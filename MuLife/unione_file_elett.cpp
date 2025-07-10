#include <iostream>
#include <fstream>
#include <vector>
//UNisce i file di tutte le prese dati per lo spettro in energia
int unione() {
    std::vector<std::string> file_names = {"output_1405_elett.txt", "output_elettrone_1505_1605.txt", "output_elettrone_1605_1905.txt","output_elettrone_1905_2105.txt","output_elettrone_2205_2305.txt", "output_elettrone_2605.txt"}; // Lista dei file da unire
    std::ofstream output_file("elettrone_unito.txt", std::ios::app); // Apertura in modalità append

    if (!output_file) {
        std::cerr << "Errore nell'apertura del file di destinazione!" << std::endl;
        return 1;
    }

    for (const auto& file_name : file_names) {
        std::ifstream input_file(file_name);
        
        if (!input_file) {
            std::cerr << "Errore nell'apertura di " << file_name << std::endl;
            continue;
        }

        output_file << input_file.rdbuf(); // Copia il contenuto del file di input nel file di output
        input_file.close();
    }

    output_file.close();
    std::cout << "Unione completata con successo!" << std::endl;

    return 0;
}
