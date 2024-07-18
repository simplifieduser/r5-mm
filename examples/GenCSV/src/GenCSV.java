import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class GenCSV {
    // Anzahl der Elemente
    static int numberOfElements = 102400;
    // Seitengröße
    static int blocksize = 4096;
    static boolean statistikEin = true;
    static Random rand = new Random(12);


    public static void main(String[] args) {
        // java Main.java
        try {
            // Optimalerfall
            FileWriter bestcaseWriter = new FileWriter("../../optimaler_Fall.csv");
            StringBuilder string_best_case = new StringBuilder();
            for (int i = 0; i < numberOfElements; i++) {
                string_best_case.append("R,0x").append(Integer.toHexString(4 * i)).append("\n");
            }
            bestcaseWriter.write(string_best_case.toString());
            bestcaseWriter.close();

            // Schlechtesterfall
            FileWriter worstcaseWriter = new FileWriter("../../schlechtester_Fall.csv");
            StringBuilder sbW = new StringBuilder();
            for (int i = 0; i < numberOfElements; i++) { // 1 Integer Pro 4KB page-> 102400 Seiten
                sbW.append("R,0x").append(Integer.toHexString(blocksize * i)).append("\n");
            }
            worstcaseWriter.write(sbW.toString());
            worstcaseWriter.close();

            // Durchschnittliche Fälle
            durchschnittlicher("../../durchschnittlicher_guter_Fall.csv", 128, 128);
            durchschnittlicher("../../durchschnittlicher_mittlerer_Fall.csv", 512, 32);
            durchschnittlicher("../../durchschnittlicher_schlechter_Fall.csv", 1024, 16);



        } catch (IOException e) {
            System.out.println("FEHLER!");
        }
    }

    static void durchschnittlicher (String name, int averageCaseNumberOfMaxPages, int averageCaseNumberOfMaxElement) throws IOException {
        if (numberOfElements / (blocksize / 4) > averageCaseNumberOfMaxPages) throw new IllegalArgumentException();

        FileWriter averagecaseWriter = new FileWriter(name);
        HashSet<Integer> visited = new HashSet<>();
        HashMap<Integer, Integer> eintraegeProSeite = new HashMap<>();
        HashMap<Integer, Integer> individualTimesOfPrint = new HashMap<>();
        StringBuilder sbA = new StringBuilder();

        int n = 0;
        while (n < numberOfElements) {
            int page = rand.nextInt(averageCaseNumberOfMaxPages);
            int position_of_first_element = 4 * rand.nextInt(0, (blocksize / 4) - 1);
            int numIntegers = rand.nextInt(averageCaseNumberOfMaxElement);
            for (int i = 0; i < numIntegers && n < numberOfElements; i++) {
                int address = blocksize * page + position_of_first_element + i * 4;
                if (!visited.contains(address)) {
                    if (i == 0) individualTimesOfPrint.put(page, individualTimesOfPrint.getOrDefault(page, 0) + 1);
                    eintraegeProSeite.put(page, eintraegeProSeite.getOrDefault(page, 0) + 1);
                    visited.add(address);
                    sbA.append("R,0x").append(Integer.toHexString(address)).append("\n");
                    n++;
                } else {
                    break;
                }
            }
        }
        averagecaseWriter.write(sbA.toString());
        averagecaseWriter.close();

        print(eintraegeProSeite,individualTimesOfPrint);

    }
    static void print (HashMap<Integer, Integer> eintraegeProSeite, HashMap<Integer, Integer> individualTimesOfPrint) {
        if (statistikEin) {
            int tmp = 0;
            int min = Integer.MAX_VALUE;
            int max = Integer.MIN_VALUE;
            for (Integer i : eintraegeProSeite.keySet()) {
                tmp += eintraegeProSeite.get(i);
                if (eintraegeProSeite.get(i) < min) min = eintraegeProSeite.get(i);
                if (eintraegeProSeite.get(i) > max) max = eintraegeProSeite.get(i);
            }

            System.out.println("Seite x hat y einträge:\n" + eintraegeProSeite);
            System.out.println("Auf Seite x stehen y zusammenhängende Blöcke:\n" + individualTimesOfPrint);
            System.out.println("Anzahl der beschriebenen Seiten: " + eintraegeProSeite.keySet().size());
            System.out.println("\nmaximale Anzahl an Einträgen: " + max);
            System.out.println("minimale Anzahl an Einträgen: " + min);
            System.out.println("durchschnittliche Anzahl an Einträge Pro Seite: " + tmp / eintraegeProSeite.keySet().size());
        }
    }

}