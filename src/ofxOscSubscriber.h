//
//  ofxOscSubscriber.h
//
//  Created by ISHII 2bit on 2015/05/10.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#include "details/ofxpubsubosc_settings.h"
#include "details/ofxpubsubosc_type_traits.h"

#include <initializer_list>

#if OFX_PUBSUBOSC_DEBUG
#   define TYPE_DEBUG(type) ofLogNotice() << __func__ << ":" << __LINE__ << "[" << address << "], " << typeid(type).name();
#else
#   define TYPE_DEBUG(type) ;
#endif

namespace ofx {
    using namespace ofxpubsubosc;
    
    class OscSubscriberManager {
    public:
        class OscSubscriber;
        
    private:
        struct SetImplementation {
        protected:
#define define_set_arithmetic(type) \
            inline void set(ofxOscMessage &m, type &v, std::size_t offset = 0) { \
                if(m.getArgType(offset) == OFXOSC_TYPE_INT32) v = m.getArgAsInt32(offset); \
                else if(m.getArgType(offset) == OFXOSC_TYPE_INT64) v = m.getArgAsInt64(offset); \
                else if(m.getArgType(offset) == OFXOSC_TYPE_FLOAT) v = m.getArgAsFloat(offset); \
                else if(m.getArgType(offset) == OFXOSC_TYPE_DOUBLE) v = m.getArgAsDouble(offset); \
                else if(m.getArgType(offset) == OFXOSC_TYPE_STRING) v = ofToDouble(m.getArgAsString(offset)); \
            }
            
            define_set_arithmetic(bool);
            define_set_arithmetic(char);
            define_set_arithmetic(unsigned char);
            define_set_arithmetic(short);
            define_set_arithmetic(unsigned short);
            define_set_arithmetic(int);
            define_set_arithmetic(unsigned int);
            define_set_arithmetic(long);
            define_set_arithmetic(unsigned long);
            define_set_arithmetic(long long);
            define_set_arithmetic(unsigned long long);
            
            define_set_arithmetic(float);
            define_set_arithmetic(double);
#undef define_set_arithmetic
            
            inline void set(ofxOscMessage &m, std::string &v, std::size_t offset = 0) {
                if(m.getArgType(offset) == OFXOSC_TYPE_STRING) v = m.getArgAsString(offset);
                else if(m.getArgType(offset) == OFXOSC_TYPE_FLOAT) v = ofToString(m.getArgAsFloat(offset));
                else if(m.getArgType(offset) == OFXOSC_TYPE_DOUBLE) v = ofToString(m.getArgAsDouble(offset));
                else if(m.getArgType(offset) == OFXOSC_TYPE_INT32) v = ofToString(m.getArgAsInt32(offset));
                else if(m.getArgType(offset) == OFXOSC_TYPE_INT64) v = ofToString(m.getArgAsInt64(offset));
                else v = m.getArgAsString(offset);
            }
            
            inline void set(ofxOscMessage &m, ofBuffer &v, std::size_t offset = 0) {
                v = m.getArgAsBlob(offset);
            }
            
            inline void set(ofxOscMessage &m, ofColor &v, std::size_t offset = 0)      { setColor<unsigned char>(m, v, 255, offset); }
            inline void set(ofxOscMessage &m, ofShortColor &v, std::size_t offset = 0) { setColor<unsigned short>(m, v, 65535, offset); }
            inline void set(ofxOscMessage &m, ofFloatColor &v, std::size_t offset = 0) { setColor<float>(m, v, 1.0f, offset); }
            
            template <typename U>
            inline void setColor(ofxOscMessage &m, ofColor_<U> &v, U defaultValue, std::size_t offset = 0) {
                if(m.getNumArgs() == 1) {
                    set(m, v.r, offset);
                    set(m, v.g, offset);
                    set(m, v.b, offset);
                    v.a = defaultValue;
                } else if(m.getNumArgs() == 3) {
                    set(m, v.r, offset + 0);
                    set(m, v.g, offset + 1);
                    set(m, v.b, offset + 2);
                    v.a = defaultValue;
                } else {
                    set(m, v.r, offset + 0);
                    set(m, v.g, offset + 1);
                    set(m, v.b, offset + 2);
                    set(m, v.a, offset + 3);
                }
            }
            
            inline void set(ofxOscMessage &m, ofVec2f &v, std::size_t offset = 0)      { setVec<2>(m, v, offset); }
            inline void set(ofxOscMessage &m, ofVec3f &v, std::size_t offset = 0)      { setVec<3>(m, v, offset); }
            inline void set(ofxOscMessage &m, ofVec4f &v, std::size_t offset = 0)      { setVec<4>(m, v, offset); }
            inline void set(ofxOscMessage &m, ofQuaternion &v, std::size_t offset = 0) { setVec<4>(m, v, offset); }
            inline void set(ofxOscMessage &m, ofMatrix3x3 &v, std::size_t offset = 0)  { setVec<9>(m, v, offset); }
            
            template <std::size_t n, typename U>
            inline void setVec(ofxOscMessage &m, U &v, std::size_t offset = 0) {
                for(int i = 0; i < min(static_cast<std::size_t>(m.getNumArgs() - offset), n); i++) {
                    set(m, v[i], offset + i);
                }
            }
            
            inline void set(ofxOscMessage &m, ofMatrix4x4 &v, std::size_t offset = 0) {
                for(int j = 0; j < 4; j++) for(int i = 0; i < 4; i++) {
                    set(m, v(i, j), offset + 4 * j + i);
                }
            }
            
            inline void set(ofxOscMessage &m, ofRectangle &v, std::size_t offset = 0) {
                set(m, v.x,      offset + 0);
                set(m, v.y,      offset + 1);
                set(m, v.width,  offset + 2);
                set(m, v.height, offset + 3);
            }
            
            template <typename U, std::size_t size>
            inline void set(ofxOscMessage &m, U (&v)[size], std::size_t offset = 0) {
                for(int i = 0; i < min(size, (m.getNumArgs() - offset) / ofxpubsubosc::type_traits<U>::size); i++) {
                    set(m, v[i], offset + i * ofxpubsubosc::type_traits<U>::size);
                }
            }
            
            template <typename U>
            inline void set(ofxOscMessage &m, std::vector<U> &v, std::size_t offset = 0) {
                std::size_t num = (m.getNumArgs() - offset) / ofxpubsubosc::type_traits<U>::size;
                if(v.size() != num) v.resize(num);
                for(int i = 0; i < num; i++) {
                    set(m, v[i], offset + i * ofxpubsubosc::type_traits<U>::size);
                }
            }
            
#pragma mark ofParameter<T> / ofParameterGroup
            
            template <typename U>
            inline void set(ofxOscMessage &m, ofParameter<U> &p, std::size_t offset = 0) {
                U u;
                set(m, u, offset);
                p.set(u);
            }

            inline void set(ofxOscMessage &m, ofAbstractParameter &p, std::size_t offset = 0) {
#define type_convert(type_) if(p.type() == typeid(ofParameter<type_>).name()) { set(m, p.cast<type_>(), offset); return; }
                type_convert(float);
                type_convert(double);
                type_convert(int);
                type_convert(unsigned int);
                type_convert(long);
                type_convert(unsigned long);
                type_convert(ofColor);
                type_convert(ofRectangle);
                type_convert(ofVec2f);
                type_convert(ofVec3f);
                type_convert(ofVec4f);
                type_convert(ofQuaternion);
                type_convert(ofMatrix3x3);
                type_convert(ofMatrix4x4);
                
                type_convert(ofFloatColor);
                type_convert(ofShortColor);
                
                type_convert(bool);
                type_convert(char);
                type_convert(unsigned char);
                type_convert(short);
                type_convert(unsigned short);
                type_convert(long long);
                type_convert(unsigned long long);
                type_convert(ofBuffer);
                ofLogWarning("ofxOscSubscriber") << "ofAbstractParameter: Unknown type \"" << p.type() << "\", bind to " << m.getAddress() << ". we ignored.";
#undef type_convert
            }
            
            inline void set(ofxOscMessage &m, ofParameterGroup &pg, std::size_t offset = 0) {
                if(m.getArgType(0) == OFXOSC_TYPE_INT32) {
                    if(pg.size() <= m.getArgAsInt32(0)) {
                        ofLogWarning("ofxOscSubscriber") << "ofAbstractParameterGroup: not contain index \"" << m.getArgAsInt32(0) << "\"";
                        return;
                    }
                    set(m, pg.get(m.getArgAsInt32(0)), offset + 1);
                } else if(m.getArgType(0) == OFXOSC_TYPE_INT64) {
                    if(pg.size() <= m.getArgAsInt64(0)) {
                        ofLogWarning("ofxOscSubscriber") << "ofAbstractParameterGroup: not contain index \"" << m.getArgAsInt64(0) << "\"";
                        return;
                    }
                    set(m, pg.get(m.getArgAsInt64(0)), offset + 1);
                } else if(m.getArgType(0) == OFXOSC_TYPE_STRING) {
                    if(!pg.contains(m.getArgAsString(0))) {
                        ofLogWarning("ofxOscSubscriber") << "ofAbstractParameterGroup: not contain key \"" << m.getArgAsString(0) << "\"";
                        return;
                    }
                    set(m, pg.get(m.getArgAsString(0)), offset + 1);
                }
            }
        };
        
#pragma mark Parameter
        
        struct AbstractParameter {
            virtual void read(ofxOscMessage &message) = 0;
        };
        
        template <typename T>
        struct Parameter : AbstractParameter, SetImplementation {
            Parameter(T &t) : t(t) {}
            virtual void read(ofxOscMessage &message) { set(message, t); }
        
        private:
            
            T &t;
        };
        
        template <typename T, typename R>
        struct SetterFunctionParameter : AbstractParameter, SetImplementation {
            using Setter = std::function<R(T)>;
            SetterFunctionParameter(Setter setter) : setter(setter) {};
            virtual void read(ofxOscMessage &message) {
                typename remove_const_reference<T>::type t;
                set(message, t);
                setter(t);
            }
            
        private:
            Setter setter;
        };
        
        template <typename R>
        struct SetterFunctionParameter<ofxOscMessage &, R> : AbstractParameter, SetImplementation {
            using Setter = std::function<R(ofxOscMessage &)>;
            SetterFunctionParameter(Setter setter) : setter(setter) {};
            virtual void read(ofxOscMessage &message) {
                setter(message);
            }
            
        private:
            Setter setter;
        };
        
        template <typename R>
        struct SetterFunctionParameter<void, R> : AbstractParameter, SetImplementation {
            using Setter = std::function<R()>;
            SetterFunctionParameter(Setter setter) : setter(setter) {};
            virtual void read(ofxOscMessage &message) {
                setter();
            }
            
        private:
            Setter setter;
        };

        template <typename T, typename C, typename R>
        struct SetterMethodParameter : AbstractParameter, SetImplementation {
            using Method = R (C::*)(T);
            SetterMethodParameter(C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                typename remove_const_reference<T>::type t;
                set(message, t);
                (that.*setter)(t);
            }
            
        private:
            C &that;
            Method setter;
        };

        template <typename C, typename R>
        struct SetterMethodParameter<ofxOscMessage &, C, R> : AbstractParameter, SetImplementation {
            using Method = R (C::*)(ofxOscMessage &);
            SetterMethodParameter(C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                (that.*setter)(message);
            }
            
        private:
            C &that;
            Method setter;
        };
        
        template <typename C, typename R>
        struct SetterMethodParameter<void, C, R> : AbstractParameter, SetImplementation {
            using Method = R (C::*)();
            SetterMethodParameter(C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                (that.*setter)();
            }
            
        private:
            C &that;
            Method setter;
        };
        
        template <typename T, typename C, typename R>
        struct ConstSetterMethodParameter : AbstractParameter, SetImplementation {
            using Method = R (C::*)(T) const;
            ConstSetterMethodParameter(const C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                typename remove_const_reference<T>::type t;
                set(message, t);
                (that.*setter)(t);
            }
            
        private:
            const C &that;
            Method setter;
        };

        template <typename C, typename R>
        struct ConstSetterMethodParameter<ofxOscMessage &, C, R> : AbstractParameter, SetImplementation {
            using Method = R (C::*)(ofxOscMessage &) const;
            ConstSetterMethodParameter(const C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                (that.*setter)(message);
            }
            
        private:
            const C &that;
            Method setter;
        };
        
        template <typename C, typename R>
        struct ConstSetterMethodParameter<void, C, R> : AbstractParameter, SetImplementation {
            using Method = R (C::*)() const;
            ConstSetterMethodParameter(const C &that, Method setter)
            : that(that)
            , setter(setter) {};
            
            virtual void read(ofxOscMessage &message) {
                (that.*setter)();
            }
            
        private:
            const C &that;
            Method setter;
        };
        
        using ParameterRef = std::shared_ptr<AbstractParameter>;
        using OscReceiverRef = std::shared_ptr<ofxOscReceiver>;
        using Targets = std::multimap<std::string, ParameterRef>;
        
    public:
        class Identifier {
            std::string address;
            ParameterRef ref;
            int key;
            
            void invalidate() {
                address = "";
                ref = nullptr;
                key = 0;
            }
        public:
            Identifier() : address(""), ref(nullptr) {}
            Identifier(const std::string &address, const ParameterRef &ref, int key)
            : address(address)
            , ref(ref)
            , key(key) {}
            
            const int getKey() const { return key; };
            bool isValid() const { return static_cast<bool>(ref); }
            
            friend class OscSubscriber;
        };
        
        class OscSubscriber {
            Targets::const_iterator findFromTargets(const Identifier &identifier, const Targets &targets) const {
                if(!identifier.isValid()) return targets.end();
                Targets::const_iterator it = targets.find(identifier.address);
                if(it != targets.end()) {
                    for(std::size_t i = 0, size = targets.count(identifier.address); i < size; ++i, ++it) {
                        if(it->second == identifier.ref) {
                            return it;
                        }
                    }
                }
                return targets.end();
            }
            
            inline Targets::const_iterator findSubscribed(const Identifier &identifier) const {
                return findFromTargets(identifier, targets);
            }

            inline Identifier subscribe_impl(const std::string &address, ParameterRef ref) {
                Targets::iterator it = targets.insert(std::make_pair(address, ref));
                return {address, ref, port};
            }
            
        public:
            
#pragma mark subscribe
            
            template <typename T>
            inline auto subscribe(const std::string &address, T &value)
            -> typename std::enable_if<!is_callable<T>::value, Identifier>::type
            {
                return subscribe_impl(address, ParameterRef(new Parameter<T>(value)));
            }
            
            template <typename T>
            inline auto subscribe(const std::string &address, T &value)
            -> typename std::enable_if<is_callable<T>::value, Identifier>::type
            {
                TYPE_DEBUG(T);
                return subscribe(address, static_cast<typename function_info<T>::function_type>(value));
            }
            
            template <typename T>
            inline auto subscribe(const std::string &address, T &&value)
            -> typename std::enable_if<is_callable<T>::value, Identifier>::type
            {
                TYPE_DEBUG(T);
                return subscribe(address, static_cast<typename function_info<T>::function_type>(value));
            }
            
            template <typename T>
            inline auto subscribe(const std::string &address, T *value)
            -> typename std::enable_if<is_callable<T>::value, Identifier>::type
            {
                TYPE_DEBUG(T);
                return subscribe(address, static_cast<typename function_info<T>::function_type>(value));
            }
            
#pragma mark -
#pragma mark setter function

            template <typename R>
            inline Identifier subscribe(const std::string &address, std::function<R()> setter) {
                TYPE_DEBUG(T);
                return subscribe_impl(address, ParameterRef(new SetterFunctionParameter<void, R>(setter)));
            }
            
            template <typename T, typename R>
            inline Identifier subscribe(const std::string &address, std::function<R(T)> setter) {
                TYPE_DEBUG(T);
                return subscribe_impl(address, ParameterRef(new SetterFunctionParameter<T, R>(setter)));
            }
            
            template <typename T, typename R>
            inline Identifier subscribe(const std::string &address, std::function<R(T &&)> setter) {
                TYPE_DEBUG(T);
                return subscribe_impl(address, ParameterRef(new SetterFunctionParameter<T, R>(setter)));
            }
            
#pragma mark setter method
            
            template <typename T, typename C, typename R>
            inline Identifier subscribe(const std::string &address, C &that, R (C::*setter)(T)) {
                return subscribe_impl(address, ParameterRef(new SetterMethodParameter<T, C, R>(that, setter)));
            }
            
            template <typename T, typename C, typename R>
            inline Identifier subscribe(const std::string &address, C *that, R (C::*setter)(T)) {
                return subscribe_impl(address, ParameterRef(new SetterMethodParameter<T, C, R>(*that, setter)));
            }
            
            template <typename T, typename C, typename R>
            inline Identifier subscribe(const std::string &address, const C &that, R (C::*setter)(T) const) {
                return subscribe_impl(address, ParameterRef(new ConstSetterMethodParameter<T, C, R>(that, setter)));
            }

            template <typename T, typename C, typename R>
            inline Identifier subscribe(const std::string &address, const C * const that, R (C::*setter)(T) const) {
                return subscribe_impl(address, ParameterRef(new ConstSetterMethodParameter<T, C, R>(*that, setter)));
            }
            
#pragma mark unscribe
            
            inline void unsubscribe(const std::string &address) {
                targets.erase(address);
            }
            
            inline void unsubscribe(Identifier &identifier) {
                if(!identifier.isValid()) return;
                Targets::const_iterator it{findSubscribed(identifier)};
                if(it != targets.end()) {
                    targets.erase(it);
                }
                identifier.invalidate();
            }
            
            inline void unsubscribe() {
                targets.clear();
            }
            
#pragma mark leakPicker
            
            inline void setLeakPicker(ParameterRef ref) {
                leakPicker = ref;
            }
            
            template <typename R>
            inline void setLeakPicker(std::function<R(ofxOscMessage &)> callback) {
                setLeakPicker(ParameterRef(new SetterFunctionParameter<ofxOscMessage &, R>(callback)));
            }
       
            template <typename C, typename R>
            inline void setLeakPicker(C &that, R (C::*callback)(ofxOscMessage &)) {
                setLeakPicker(ParameterRef(new SetterMethodParameter<ofxOscMessage &, C, R>(that, callback)));
            }
            
            template <typename C, typename R>
            inline void setLeakPicker(C *that, R (C::*callback)(ofxOscMessage &)) {
                setLeakPicker(ParameterRef(new SetterMethodParameter<ofxOscMessage &, C, R>(*that, callback)));
            }
            
            template <typename C, typename R>
            inline void setLeakPicker(const C &that, R (C::*callback)(ofxOscMessage &) const) {
                setLeakPicker(ParameterRef(new ConstSetterMethodParameter<ofxOscMessage &, C, R>(that, callback)));
            }
            
            template <typename C, typename R>
            inline void setLeakPicker(const C * const that, R (C::*callback)(ofxOscMessage &) const) {
                setLeakPicker(ParameterRef(new ConstSetterMethodParameter<ofxOscMessage &, C, R>(*that, callback)));
            }
            
            
            inline void removeLeakPicker() {
                leakPicker.reset();
            }
            
            inline bool isSubscribed() const {
                return !targets.empty();
            }
            
            inline bool isSubscribed(const std::string &address) const {
                return isSubscribed() && (targets.find(address) != targets.end());
            }
            
            inline bool isLeakedOscCovered() const {
                return static_cast<bool>(leakPicker);
            }
            
            void clearLeakedOscMessages() {
                std::queue<ofxOscMessage> empty;
                std::swap(leakedOscMessages, empty);
            }
            
            inline bool hasWaitingLeakedOscMessages() const {
                return !static_cast<bool>(leakPicker) && !leakedOscMessages.empty();
            }
            
            inline bool getNextLeakedOscMessage(ofxOscMessage &m) {
                if(hasWaitingLeakedOscMessages()) {
                    m.copy(leakedOscMessages.front());
                    leakedOscMessages.pop();
                    return true;
                } else {
                    return false;
                }
            }
            
            void notify(ofxOscMessage & m) {
                const std::string &address = m.getAddress();
                Targets::iterator it = targets.find(address);
                if(it != targets.end()) {
                    for(std::size_t i = 0; i < targets.count(address); i++, ++it) {
                        it->second->read(m);
                    }
                }
            }
            
            void notify(const Identifier &identifier, ofxOscMessage &m) {
                if(!identifier.isValid()) return;
                Targets::const_iterator it{findSubscribed(identifier)};
                if(it != targets.end() && it->first == m.getAddress()) {
                    it->second->read(m);
                }
            }
            
            using Ref = std::shared_ptr<OscSubscriber>;
        private:
            OscSubscriber(int port)
            : port(port) {
                receiver.setup(port);
            }
            
            void update() {
                clearLeakedOscMessages();
                ofxOscMessage m;
                while(receiver.hasWaitingMessages()) {
                    receiver.getNextMessage(m);
                    const std::string &address = m.getAddress();
                    Targets::iterator it = targets.find(address);
                    if(it != targets.end()) {
                        for(std::size_t i = 0; i < targets.count(address); i++, ++it) {
                            it->second->read(m);
                        }
                    } else {
                        if(leakPicker) {
                            leakPicker->read(m);
                        } else {
                            leakedOscMessages.push(m);
                        }
                    }
                }
            }
            
            int port;
            ofxOscReceiver receiver;
            Targets targets;
            ParameterRef leakPicker;
            std::queue<ofxOscMessage> leakedOscMessages;
            
            friend class OscSubscriberManager;
        };
        
        static OscSubscriberManager &getSharedInstance() {
            static OscSubscriberManager *sharedInstance = new OscSubscriberManager;
            return *sharedInstance;
        }
        
        static OscSubscriber &getOscSubscriber(int port) {
            OscSubscribers &managers = getSharedInstance().managers;
            if(managers.find(port) == managers.end()) {
                managers.insert(std::make_pair(port, OscSubscriber::Ref(new OscSubscriber(port))));
            }
            return *(managers[port].get());
        }
        
    private:
        using OscSubscribers = std::map<int, OscSubscriber::Ref>;
        void update(ofEventArgs &args) {
            for(OscSubscribers::iterator it = managers.begin(); it != managers.end(); ++it) {
                it->second->update();
            }
        }
        
        OscSubscriberManager() {
            ofAddListener(ofEvents().update, this, &OscSubscriberManager::update, OF_EVENT_ORDER_BEFORE_APP);
        }
        
        virtual ~OscSubscriberManager() {
            ofRemoveListener(ofEvents().update, this, &OscSubscriberManager::update, OF_EVENT_ORDER_BEFORE_APP);
        }
        OscSubscribers managers;
        
#pragma mark iterator
    public:
        using iterator = OscSubscribers::iterator;
        using const_iterator = OscSubscribers::const_iterator;
        using reverse_iterator = OscSubscribers::reverse_iterator;
        using const_reverse_iterator = OscSubscribers::const_reverse_iterator;
        
        iterator begin() { return managers.begin(); }
        iterator end() { return managers.end(); }
        
        const_iterator begin() const { return managers.cbegin(); }
        const_iterator end() const { return managers.cend(); }
        const_iterator cbegin() const { return managers.cbegin(); }
        const_iterator cend() const { return managers.cend(); }
        
        reverse_iterator rbegin() { return managers.rbegin(); }
        reverse_iterator rend() { return managers.rend(); }
        
        const_reverse_iterator rbegin() const { return managers.crbegin(); }
        const_reverse_iterator rend() const { return managers.crend(); }
        const_reverse_iterator crbegin() const { return managers.crbegin(); }
        const_reverse_iterator crend() const { return managers.crend(); }
    };
};

using ofxOscSubscriberManager = ofx::OscSubscriberManager;
using ofxOscSubscriber = ofxOscSubscriberManager::OscSubscriber;
using ofxOscSubscriberIdentifier = ofxOscSubscriberManager::Identifier;

/// \brief get a OscSubscriberManager.
/// \returns ofxOscSubscriberManager

inline ofxOscSubscriberManager &ofxGetOscSubscriberManager() {
    return ofxOscSubscriberManager::getSharedInstance();
}

/// \brief get a OscSubscriber.
/// \param port binded port is typed int
/// \returns ofxOscSubscriber binded to port

inline ofxOscSubscriber &ofxGetOscSubscriber(int port) {
    return ofxOscSubscriberManager::getOscSubscriber(port);
}

#pragma mark interface about subscribe

/// \name ofxSubscribeOsc
/// \{

/// \brief bind a referece of value to the argument(s) of OSC messages with an address pattern _address_ incoming to _port_.
/// \param port binded port is typed int
/// \param address osc address is typed const std::string &
/// \param value reference of value is typed T &
/// \returns ofxOscSubscriberIdentifier

#pragma mark reference

template <typename T>
inline ofxOscSubscriberIdentifier ofxSubscribeOsc(int port, const std::string &address, T &&value) {
    return ofxGetOscSubscriber(port).subscribe(address, value);
}

#pragma mark setter function/method

/// \brief bind a callback to the OSC messages with an address pattern _address_ incoming to _port_.
/// \param port binded port is typed int
/// \param address osc address is typed const std::string &
/// \param callback is kicked when receive a message to address
/// \returns ofxOscSubscriberIdentifier

template <typename Ret, typename Arg>
inline ofxOscSubscriberIdentifier ofxSubscribeOsc(int port, const std::string &address, std::function<Ret(Arg)> callback) {
    return ofxGetOscSubscriber(port).subscribe(address, callback);
}

/// \brief bind a callback to the OSC messages with an address pattern _address_ incoming to _port_.
/// \param port binded port is typed int
/// \param address osc address is typed const std::string &
/// \param that this object is typed ObjectRefOrPtr is reference or pointer of object, will bind with next argument of parameter method. is called as (that.*getter)(message) when receive a message.
/// \param callback is kicked when receive a message to address
/// \returns ofxOscSubscriberIdentifier

template <typename ObjectRefOrPtr, typename Method>
inline ofxOscSubscriberIdentifier ofxSubscribeOsc(int port, const std::string &address, ObjectRefOrPtr &&that, Method callback) {
    return ofxGetOscSubscriber(port).subscribe(address, that, callback);
}

#pragma mark subscribe multiple port at once

template <typename ... Args>
inline void ofxSubscribeOsc(const std::initializer_list<int> &ports, const std::string &address, Args & ... args) {
    for(auto &port : ports) {
        ofxSubscribeOsc(port, address, args ...);
    }
}

template <typename ... Args>
inline void ofxSubscribeOsc(int port, const std::initializer_list<const std::string> &addresses, Args & ... args) {
    auto &subscriber = ofxGetOscSubscriber(port);
    for(auto &address : addresses) {
        subscriber.subscribe(address, args ...);
    }
}

template <typename ... Args>
inline void ofxSubscribeOsc(const std::initializer_list<int> &ports, const std::initializer_list<const std::string> &addresses, Args & ... args) {
    for(auto &port : ports) {
        ofxSubscribeOsc(port, addresses, args ...);
    }
}

/// \}

#pragma mark unsubscribe

/// \name ofxUnsubscribeOsc
/// \{

// TODO: add document

inline void ofxUnsubscribeOsc(ofxOscSubscriberIdentifier &identifier) {
    if(!identifier.isValid()) return;
    ofxGetOscSubscriber(identifier.getKey()).unsubscribe(identifier);
}


/// \brief unbind from OSC messages with an address pattern _address_ incoming to _port_.
/// \param port binded port is typed int
/// \param address osc address is typed const std::string &
/// \returns void

inline void ofxUnsubscribeOsc(int port, const std::string &address) {
    ofxGetOscSubscriber(port).unsubscribe(address);
}

/// \brief unbind from OSC messages with any address patterns incoming to _port_.
/// \param port binded port is typed int
/// \returns void

inline void ofxUnsubscribeOsc(int port) {
    ofxGetOscSubscriber(port).unsubscribe();
}

/// \brief unbind from all OSC messages.
/// \returns void

inline void ofxUnsubscribeOsc() {
    ofxOscSubscriberManager &manager = ofxGetOscSubscriberManager();
    ofxOscSubscriberManager::iterator it  = manager.begin(),
                                      end = manager.end();
    for(; it != end; ++it) {
        it->second->unsubscribe();
    }
}

/// \}

#pragma mark notify messages manually

inline void ofxNotifyToSubscribedOsc(ofxOscSubscriberIdentifier &identifier, ofxOscMessage &m) {
    ofxGetOscSubscriber(identifier.getKey()).notify(m);
}

inline void ofxNotifyToSubscribedOsc(int port, ofxOscMessage &m) {
    ofxGetOscSubscriber(port).notify(m);
}

inline void ofxNotifyToSubscribedOsc(ofxOscMessage &m) {
    ofxOscSubscriberManager &manager = ofxGetOscSubscriberManager();
    ofxOscSubscriberManager::iterator it  = manager.begin(),
                                      end = manager.end();
    for(; it != end; ++it) {
        it->second->notify(m);
    }
}

#pragma mark interface about leaked osc

/// \name ofxSetLeakedOscPicker
/// \{

/// \brief bind a callback to the OSC messages with are not match other patterns incoming to port.
/// \param port binded port is typed int
/// \callback is kicked when receive a leaked addresses
/// \returns void

template <typename Ret>
inline void ofxSetLeakedOscPicker(int port, Ret(*callback)(ofxOscMessage &)) {
    ofxGetOscSubscriber(port).setLeakPicker(callback);
}

/// \brief bind a callback to the OSC messages with are not match other patterns incoming to port.
/// \param port binded port is typed int
/// \param that this object is typed Obj&, will bind with next argument of parameter method. is called as (that.*getter)(message) when receive a message.
/// \callback is kicked when receive a leaked addresses
/// \returns void

template <typename Obj, typename Ret>
inline void ofxSetLeakedOscPicker(int port, Obj &that, Ret(Obj::*callback)(ofxOscMessage &)) {
    ofxGetOscSubscriber(port).setLeakPicker(that, callback);
}

/// \brief bind a callback to the OSC messages with are not match other patterns incoming to port.
/// \param port binded port is typed int
/// \param that this object is typed Obj*, will bind with next argument of parameter method. is called as (that->*getter)(message) when receive a message.
/// \callback is kicked when receive a leaked addresses
/// \returns void

template <typename Obj, typename Ret>
inline void ofxSetLeakedOscPicker(int port, Obj *that, Ret(Obj::*callback)(ofxOscMessage &)) {
    ofxGetOscSubscriber(port).setLeakPicker(*that, callback);
}

/// \brief bind a callback to the OSC messages with are not match other patterns incoming to port.
/// \param port binded port is typed int
/// \param that this object is typed Obj&, will bind with next argument of parameter method. is called as (that.*getter)(message) when receive a message.
/// \callback is kicked when receive a leaked addresses
/// \returns void

template <typename Obj, typename Ret>
inline void ofxSetLeakedOscPicker(int port, const Obj &that, Ret(Obj::*callback)(ofxOscMessage &) const) {
    ofxGetOscSubscriber(port).setLeakPicker(that, callback);
}

/// \brief bind a callback to the OSC messages with are not match other patterns incoming to port.
/// \param port binded port is typed int
/// \param that this object is typed Obj*, will bind with next argument of parameter method. is called as (that->*getter)(message) when receive a message.
/// \callback is kicked when receive a leaked addresses
/// \returns void

template <typename Obj, typename Ret>
inline void ofxSetLeakedOscPicker(int port, const Obj *that, Ret(Obj::*callback)(ofxOscMessage &) const) {
    ofxGetOscSubscriber(port).setLeakPicker(*that, callback);
}

inline void ofxSetLeakedOscPicker(int port, const std::function<void(ofxOscMessage &)> &callback) {
    ofxGetOscSubscriber(port).setLeakPicker(callback);
}
#pragma mark leak picking all port

template <typename T, typename ... Args>
inline typename std::enable_if<!std::is_integral<T>::value>::type ofxSetLeakedOscPickerAll(T &arg, Args & ... args) {
    for(auto subscriber : ofxGetOscSubscriberManager()) {
        subscriber.second->setLeakPicker(arg, args ...);
    }
}

#pragma mark remove leaked osc picker(s)

/// \brief remove a callback receives messages has leaked patterns incoming to port.
/// \param port binded port is typed int
/// \returns void

inline void ofxRemoveLeakedOscPicker(int port) {
    ofxGetOscSubscriber(port).removeLeakPicker();
}

inline void ofxRemoveLeakedOscPicker() {
    ofxOscSubscriberManager &manager = ofxGetOscSubscriberManager();
    ofxOscSubscriberManager::iterator it  = manager.begin(),
                                      end = manager.end();
    for(; it != end; ++it) {
        it->second->removeLeakPicker();
    }
}

/// \}

#define SubscribeOsc(port, name) ofxSubscribeOsc(port, "/" #name, name)
