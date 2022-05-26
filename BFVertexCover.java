/* Brute Force Vertex Cover */
import java.util.HashSet;
import java.util.Set;

public class Graph
{
    private int L;
    private Set<Integer> G;
    
    Graph(int size)
    {
        L = size; //Length
        G = new HashSet<>(); //Graph
    }
    void addEdge(int v, int w)
    {
        int edge = 0;
        edge |= (1 << v);
        edge |= (1 << w);
        G.add(edge);
    }
    int findVertexCover() {
        int possibility = (1 << L) - 1;
        for (int i=1; i<=possibility; i++) {
            //Check each possible case.
            boolean ok = true;
            for (int j : G) {
                if ((j & i) == 0) {
                    ok = false;
                }
            }
            if (ok == true) {
                return i;
            }
        }
        return 0;
    }
    void printVertexCover()
    {
        int case_id = findVertexCover();
        System.out.print("Exact Solution: ");
        if (case_id > 0) {
            for (int i=0; i<L; i++) {
                if ((case_id & (1 << i)) != 0) {
                    System.out.print(i+" ");
                }
            }
        }
        System.out.println();
    }
    
    public static void main(String args[])
    {
        Graph g = new Graph(10);
        g.addEdge(0, 1);
        g.addEdge(2, 3);
        g.addEdge(4, 5);
        g.addEdge(6, 7);
        g.addEdge(8, 9);
        g.printVertexCover();
    }
}
