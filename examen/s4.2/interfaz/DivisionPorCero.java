// interfaz/DivisionPorCero.java
import java.rmi.RemoteException;

public class DivisionPorCero extends RemoteException {
    public DivisionPorCero (String message) {
        super(message);
    }
}