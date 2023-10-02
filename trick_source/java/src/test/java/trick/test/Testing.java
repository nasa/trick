package trick.test;
import static org.junit.Assert.assertTrue;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
public class Testing  {
    @BeforeClass
    public static void beforeClassmethod() {
        System.out.println("Hi welcome in Before Class Method");
    }
    @Before
    public void beforemethod() {
        System.out.println("Hello there welcome in Before Test Case strategy ");
    }
    @Test
    public void Test(){
        System.out.println("Welcome in First Test");
        int num = 5;
        assertTrue(num == 5);
    }
    @After
    public void aftermethod() {
        System.out.println("Hi welcome in After Test Case");
    }
    @AfterClass
    public static void afterClassmethod() {
        System.out.println("Hi welcome in After Class");
    }
}