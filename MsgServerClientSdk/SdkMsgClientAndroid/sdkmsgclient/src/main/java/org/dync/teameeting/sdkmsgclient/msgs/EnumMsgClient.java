package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/12/16.
 */
public class EnumMsgClient {
    public enum EMSCmd {
        MSADD_GROUP(0),
        MSRMV_GROUP(1),
        MSFETCH_SEQN(2);

        public static EMSCmd forNumber(int value) {
            switch (value) {
                case 0: return MSADD_GROUP;
                case 1: return MSRMV_GROUP;
                case 2: return MSFETCH_SEQN;
                default: return null;
            }
        }

        public final int getNumber() {
            return value;
        }

        private final int value;

        private EMSCmd(int value) {
            this.value = value;
        }
    }
}
