import java.io.*;
import java.util.ArrayList;

public class ReadToCsv {
    static ArrayList<String> data = new ArrayList<String>();
    public static void main(String[] args) {
        int nline=read("res/acc_output.txt");
        write("output_table.csv",nline);
    }

    /**
     * File reading function
     */
    public static int read(String filename){
        int nline=0;
        try (BufferedReader br =
                     new BufferedReader(new FileReader(filename))) {

            String text;

                while ((text = br.readLine()) != null) {
                    // code goes here
                    String[] dataLine = text.split(" ");
                    switch (Math.floorMod(nline,9)){
                        case 0:
                        case 8:
                            data.add(dataLine[1]);
                            break;
                        case 1:
                        case 3:
                        case 6:
                        case 7:
                            data.add(dataLine[2]);
                            break;
                        case 2:
                            data.add(dataLine[1]);
                            break;
                        case 4:
                        case 5:
                            data.add(dataLine[3]);
                            break;
                        default:
                            break;
                    }
                    nline++;
                }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return nline;
    }
    /**
     * File writing function
     * @param s
     */
    public static void write(String s, int nline){
        try (PrintWriter pw =
                     new PrintWriter(new FileWriter(s))) {
            //code goes here
            pw.write("Level,Propagation,Budget,Max Depth,Total Expanded Nodes,Total generated nodes" +
                    ",Time passed,Expanded nodes/second,Score\n");
            for(int i=0;i<nline/9;i++){
                for (int j=0;j<9;j++){
                    pw.format(data.get(i*9+j)+",");
                }
                pw.write("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
