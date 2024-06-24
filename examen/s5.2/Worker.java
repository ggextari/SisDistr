import com.rabbitmq.client.*;
import java.io.IOException;

public class Worker {
    private final static String QUEUE_NAME = "task_queue";

    private static void doWork(String task) throws InterruptedException
    {
        for (char ch: task.toCharArray()) {
            if (ch == '.') 
            {
                try
                {
                    Thread.sleep(1000);
                }
                catch (InterruptedException _ignored)
                {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public static void main(String[] argv) throws Exception {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("localhost");
        Connection connection = factory.newConnection();
        Channel channel = connection.createChannel();

        boolean durable = true;
        channel.queueDeclare(QUEUE_NAME, durable, false, false, null);
        System.out.println(" [*] Waiting for messages. To exit press CTRL+C");

        int prefetchCount = 1;
        channel.basicQos(prefetchCount); // acepta solo un mensaje sin ack de cada vez
        final Consumer consumer = new DefaultConsumer(channel) {
            @Override
            public void handleDelivery(String consumerTag, Envelope envelope,
            AMQP.BasicProperties properties, byte[] body)
            throws IOException
            {
                String message = new String(body, "UTF-8");
                System.out.println(" [x] Received '" + message + "'");
                try {
                    doWork(message);
                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                }
                finally{
                    System.out.println(" [x] Done");
                    //confirmación del procesamiento del mensaje
                    channel.basicAck(envelope.getDeliveryTag(), false);
                }
            }
        };
        boolean autoAck = false; // acknowledgement
        channel.basicConsume(QUEUE_NAME, autoAck, consumer);
    }
}