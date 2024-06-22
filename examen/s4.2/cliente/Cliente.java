import java.rmi.Naming;
import java.rmi.RemoteException;
import java.io.*;

public class Cliente {
    public static void main(String[] args) {
        double op1;
        double op2;
        double res;

        try
        {
            Calculadora obj = (Calculadora) Naming.lookup("CalculadoraServer");
            BufferedReader in =
            new BufferedReader (new InputStreamReader (System.in));
            try {
                System.out.println("Operando1: ");
                op1 = Double.parseDouble(in.readLine());
                System.out.println("Operando2: ");
                op2 = Double.parseDouble(in.readLine());
                
                res = obj.suma((int) op1, (int) op2);
                System.out.println("El resultado de la suma es: " + res);
                
                res = obj.resta((int) op1, (int) op2);
                System.out.println("El resultado de la resta es: " + res);

                res = obj.multiplicacion(op1, op2);
                System.out.println("El resultado de la multiplicación es: " + res);
                
                try {
                    res = obj.division(op1, op2);
                    System.out.println("El resultado de la división es: " + res);
                } catch (DivisionPorCero e) {
                    System.out.println("Error: " + e.getMessage());
                }
            }
            catch (RemoteException e) {
                System.out.println("Error en la invocación remota: " +
                e.getMessage());
            }
            catch (Exception e) {
                System.out.println("Error en lectura de datos: " +
                e.getMessage());
            }
        }
        catch (Exception e) {
            System.out.println("Error en lookup: " + e.getMessage());
        } // fin del primer try
    } // fin del main
} // fin de la clase