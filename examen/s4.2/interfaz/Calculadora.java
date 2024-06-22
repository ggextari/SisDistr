import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Calculadora extends Remote {
    int suma (int o1, int o2) throws RemoteException;
    int resta (int o1, int o2) throws RemoteException;
    double multiplicacion(double o1, double o2) throws RemoteException;
    double division(double o1, double o2) throws RemoteException, DivisionPorCero;
}