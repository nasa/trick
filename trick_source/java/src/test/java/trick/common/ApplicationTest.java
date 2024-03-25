package trick.common;

public class ApplicationTest {
    
    public static String getTrickHome() {
        String path;
        
        // Getting TRICK_HOME environment variable
        path = System.getenv("TRICK_HOME");
        
        // Getting Trick's home directory if TRICK_HOME isn't set
        if(path != null) {
            if(path.endsWith("/"))
                path.substring(0, path.length() - 1);
        } else {
            path = System.getProperty("user.dir");
            int cutoff = path.indexOf("/trick_source");
            path = (cutoff > 0) ? path.substring(0, cutoff) : "";
        }

        return path;
    }

	public boolean compStringArray(String[] a, String[] b) {
		boolean same = true;
		for(int i = 0; i < a.length && i < b.length; i++) {
			same = same && sameLetters(a[i], b[i]);
			if(!same) return same;
		}
		return same;
	}

	public boolean sameLetters(String str1, String str2) {
		String a = str1.replaceAll("\\s+", "").toLowerCase(),
			   b = str2.replaceAll("\\s+", "").toLowerCase();
		return a.equals(b);
	}

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}
}
