import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class CalculadoraImpl extends UnicastRemoteObject implements Calculadora {
    
    public CalculadoraImpl() throws RemoteException {
        super();
    }

    public int suma (int op1, int op2) {
        return op1 + op2;
    }

    public int resta (int op1, int op2) {
        return op1 - op2;
    }

    public double multiplicacion(double op1, double op2) {
        return op1 * op2;
    }

    public double division(double op1, double op2) throws RemoteException, DivisionPorCero {
        if (op2 == 0) {
            throw new DivisionPorCero("No se puede dividir por cero.");
        }
        return op1 / op2;
    }
}