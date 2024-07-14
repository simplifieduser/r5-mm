import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class Main {
    public static void main(String[] args) {
     // TODO runt with java Main.java!!!!!!!!!!!
        int numberOfElements = 102400; // <-- diese Zahl ändern um Anzahl der zu schreibenden Elemente zu ändern
        int averageCaseNumberOfMaxPages = 2048; // <-- diese Zahl ändern um maximale Anzahl der beschriebenen Seiten im durchschnittsfall
        int averageCaseNumberOfMaxElement = 8;// <-- diese Zahl ändern um maximale Anzahl der zusammenhängenden Integer, pro schreib Aufruf zu setzen
        int blocksize = 4096; // <-- Zahl entspricht dem Parameter --blocksize

        if(numberOfElements  / (blocksize/4) > averageCaseNumberOfMaxPages) throw new IllegalArgumentException();


        try {
            // Best case
            FileWriter bestcaseWriter = new FileWriter("../../optimaler_Fall.csv");
            StringBuilder sbB = new StringBuilder();
            for (int i = 0; i < numberOfElements; i++) { // 1024 Integer pro 4KB-Seite -> 100 Seiten
                sbB.append("R,0x").append(Integer.toHexString(4 * i)).append("\n");
            }
            bestcaseWriter.write(sbB.toString());
            bestcaseWriter.close();

            // Worst case
            FileWriter worstcaseWriter = new FileWriter("../../schlechtester_Fall.csv");
            StringBuilder sbW = new StringBuilder();
            for (int i = 0; i < numberOfElements; i++) { // 1 Integer Pro 4KB page-> 102400 Seiten
                sbW.append("R,0x").append(Integer.toHexString(blocksize * i)).append("\n");
            }
            worstcaseWriter.write(sbW.toString());
            worstcaseWriter.close();

            // Average case
            FileWriter averagecaseWriter = new FileWriter("../../durchschnittlicher_schlechter_Fall.csv");
            Random rand = new Random(12);
            HashSet<Integer> visited = new HashSet<>();
            HashMap<Integer,Integer> eintraegeProSeite = new HashMap<>();
            HashMap<Integer,Integer> individualTimesOfPrint = new HashMap<>();
            StringBuilder sbA = new StringBuilder();

            int n = 0;
            while (n < numberOfElements) {
                int page = rand.nextInt(averageCaseNumberOfMaxPages);
                int startingpos = 4*rand.nextInt(0, (blocksize/4)-1);


                int numIntegers = rand.nextInt( Math.min((blocksize - startingpos) / 4,averageCaseNumberOfMaxElement)); //gibt an wie viele Integers maximal aufeinanderfolgend stehen (4096 - startingpos) /4, um keine Seiten overflow zu haben

                for (int i = 0; i < numIntegers && n < numberOfElements; i++) {
                    int address = blocksize * page + startingpos + i * 4;
                    if (!visited.contains(address)) {
                        if(i==0) individualTimesOfPrint.put(page,individualTimesOfPrint.getOrDefault(page,0)+1);

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

            // durchschnittliche Einträge pro Seite

            int tmp = 0;
            int min = Integer.MAX_VALUE;
            int max = Integer.MIN_VALUE;
            for (Integer i : eintraegeProSeite.keySet()){
                tmp += eintraegeProSeite.get(i);
                if (eintraegeProSeite.get(i) < min) min = eintraegeProSeite.get(i);
                if (eintraegeProSeite.get(i) > max) max = eintraegeProSeite.get(i);
            }

            System.out.println("Seite x hat y einträge:\n" + eintraegeProSeite);
            System.out.println("Auf Seite x stehen y zusammenhängende Blöcke:\n"+individualTimesOfPrint);
            System.out.println("Anzahl der beschriebenen Seiten: "+eintraegeProSeite.keySet().size());
            System.out.println("\nmaximale Anzahl an Einträgen: "+max);
            System.out.println("minimale Anzahl an Einträgen: "+min);
            System.out.println("durchschnittliche Anzahl an Einträge Pro Seite: "+ tmp / eintraegeProSeite.keySet().size());
            // durch ändern des maximalen Zufallswertes in page wird die Anzahl an möglichen Seiten verändert
            // durch ändern des maximalen Zufallswertes in startingPos wird die maximale größe eines Blockes verändert


        } catch (IOException e) {
            System.out.println("FEHLER!");
        }
    }
}