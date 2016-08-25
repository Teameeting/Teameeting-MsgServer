package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public final class MSSubMessage {
    public static final class MSTxtMessage {
        private MSMessage _message = null;

        public MSTxtMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public String getContent() {
            return _message.getContent();
        }

        public void setContent(String content) {
            _message.setContent(content);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }
    }

    public static final class MSLivMessage {
        private MSMessage _message = null;

        public MSLivMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public int getFlag() {
            return _message.getFlag();
        }

        public void setFlag(int flag) {
            _message.setFlag(flag);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getToNickName() { return _message.getToNickName(); }

        public void setToNickName(String toNickName) { _message.setToNickName(toNickName); }
    }

    public static final class MSRenMessage {
        private MSMessage _message = null;

        public MSRenMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getCash() {
            return _message.getCash();
        }

        public void setCash(String cash) {
            _message.setCash(cash);
        }

        public String getWishcont() {
            return _message.getWishcont();
        }

        public void setWishcont(String wishcont) {
            _message.setWishcont(wishcont);
        }

        public String getToNickName() { return _message.getToNickName(); }

        public void setToNickName(String toNickName) { _message.setToNickName(toNickName); }
    }

    public static final class MSBlkMessage {
        private MSMessage _message = null;

        public MSBlkMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public int getFlag() {
            return _message.getFlag();
        }

        public void setFlag(int flag) {
            _message.setFlag(flag);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getToNickName() { return _message.getToNickName(); }

        public void setToNickName(String toNickName) { _message.setToNickName(toNickName); }
    }

    public static final class MSFbdMessage {
        private MSMessage _message = null;

        public MSFbdMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public int getFlag() {
            return _message.getFlag();
        }

        public void setFlag(int flag) {
            _message.setFlag(flag);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getToNickName() { return _message.getToNickName(); }

        public void setToNickName(String toNickName) { _message.setToNickName(toNickName); }
    }

    public static final class MSMgrMessage  {
        private MSMessage _message;

        public MSMgrMessage() {
            _message = new MSMessage();
        }

        public int getPush() {
            return _message.getPush();
        }

        public void setPush(int push) {
            _message.setPush(push);
        }

        public int getFlag() {
            return _message.getFlag();
        }

        public void setFlag(int flag) {
            _message.setFlag(flag);
        }

        public String getToId() {
            return _message.getToId();
        }

        public void setToId(String toId) {
            _message.setToId(toId);
        }

        public String getGroupId() {
            return _message.getGroupId();
        }

        public void setGroupId(String groupId) {
            _message.setGroupId(groupId);
        }

        public String getToNickName() { return _message.getToNickName(); }

        public void setToNickName(String toNickName) { _message.setToNickName(toNickName); }
    }
}
