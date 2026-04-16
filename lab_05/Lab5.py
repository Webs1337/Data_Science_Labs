import streamlit as st
import pandas as pd
import numpy as np
import plotly.express as px
import os
import urllib.request
from datetime import datetime

# Налаштування сторінки
st.set_page_config(page_title="Агрокліматичний аналіз NOAA", page_icon="🌿", layout="wide")

# Автоматичне завантаження та очищення
@st.cache_data(show_spinner=False)
def fetch_and_clean_data():
    data_dir = "vhi_dataset"
    os.makedirs(data_dir, exist_ok=True)
    
    downloaded_files = [f for f in os.listdir(data_dir) if f.endswith(".csv")]
    if len(downloaded_files) < 27:
        with st.spinner('Ініціалізація бази даних. Стягування архівів NOAA...'):
            progress_bar = st.progress(0)
            for i, prov_id in enumerate(range(1, 28)):
                if not any(f.startswith(f"vhi_{prov_id}_") for f in os.listdir(data_dir)):
                    url = f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={prov_id}&year1=1981&year2=2024&type=Mean"
                    timestamp = datetime.now().strftime("%Y%m%d%H%M")
                    filepath = os.path.join(data_dir, f"vhi_{prov_id}_{timestamp}.csv")
                    try:
                        urllib.request.urlretrieve(url, filepath)
                    except Exception as e:
                        st.error(f"Збій з'єднання для ID {prov_id}: {e}")
                progress_bar.progress((i + 1) / 27)
            progress_bar.empty()

    noaa_to_ua = {
        1: 22, 2: 24, 3: 23, 4: 25, 5: 3, 6: 4, 7: 8, 8: 19, 9: 20, 10: 21,
        11: 9, 12: 26, 13: 10, 14: 11, 15: 12, 16: 13, 17: 14, 18: 15, 19: 16,
        20: 27, 21: 17, 22: 18, 23: 6, 24: 1, 25: 2, 26: 7, 27: 5
    }
    
    regions_map = {
        1: 'Вінницька', 2: 'Волинська', 3: 'Дніпропетровська', 4: 'Донецька',
        5: 'Житомирська', 6: 'Закарпатська', 7: 'Запорізька', 8: 'Івано-Франківська',
        9: 'Київська', 10: 'Кіровоградська', 11: 'Луганська', 12: 'Львівська',
        13: 'Миколаївська', 14: 'Одеська', 15: 'Полтавська', 16: 'Рівненська',
        17: 'Сумська', 18: 'Тернопільська', 19: 'Харківська', 20: 'Херсонська',
        21: 'Хмельницька', 22: 'Черкаська', 23: 'Чернівецька', 24: 'Чернігівська',
        25: 'АР Крим', 26: 'м. Київ', 27: 'м. Севастополь'
    }

    dataframes = []
    for filename in os.listdir(data_dir):
        if not filename.endswith(".csv"): continue
        try:
            raw_id = int(filename.split('_')[1])
            correct_id = noaa_to_ua.get(raw_id)
        except (IndexError, ValueError):
            continue
            
        if correct_id is None: continue
        
        filepath = os.path.join(data_dir, filename)
        df = pd.read_csv(filepath, header=1, names=['Year', 'Week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI', 'Drop'], skipinitialspace=True)
        df.drop(columns=['Drop'], errors='ignore', inplace=True)
        
        df['Year'] = df['Year'].astype(str).str.replace(r'<[^>]+>', '', regex=True).str.strip()
        df = df[df['Year'].str.isnumeric()].copy()
        
        df['Year'] = df['Year'].astype(int)
        df['Week'] = df['Week'].astype(int)
        
        for col in ['VCI', 'TCI', 'VHI']:
            df[col] = pd.to_numeric(df[col], errors='coerce')
            
        df = df[(df['VHI'] != -1.0) & (df['Week'] <= 52)]
        df['Region'] = regions_map[correct_id]
        
        dataframes.append(df)

    return pd.concat(dataframes, ignore_index=True) if dataframes else pd.DataFrame()

df_main = fetch_and_clean_data()

if df_main.empty:
    st.error("Критична помилка: Дані відсутні або пошкоджені.")
    st.stop()

# Керування станом
def init_state():
    defaults = {
        'target_idx': 'VHI',
        'target_reg': sorted(df_main['Region'].unique())[0],
        'wk_slider': (1, 52),
        'yr_slider': (int(df_main['Year'].min()), int(df_main['Year'].max())),
        'sort_up': False,
        'sort_down': False
    }
    for k, v in defaults.items():
        if k not in st.session_state:
            st.session_state[k] = v

init_state()

def hard_reset():
    for key in ['target_idx', 'target_reg', 'wk_slider', 'yr_slider', 'sort_up', 'sort_down']:
        del st.session_state[key]
    init_state()

# Візуальний інтерфейс
st.title("🌿 Агрокліматичний аналіз NOAA")
st.markdown("<p style='font-size: 1.1rem; color: #64748b;'>Система інтерактивного моніторингу вегетаційних індексів України</p>", unsafe_allow_html=True)
st.markdown("---")

col_control, col_display = st.columns([1, 3], gap="large")

with col_control:
    st.markdown("### Параметри аналізу")
    
    st.selectbox("Ключовий показник:", ['VCI', 'TCI', 'VHI'], key='target_idx')
    st.selectbox("Область дослідження:", sorted(df_main['Region'].unique()), key='target_reg')
    
    st.markdown("<br>", unsafe_allow_html=True)
    
    st.slider("Інтервал тижнів:", 1, 52, key='wk_slider')
    st.slider("Період (роки):", int(df_main['Year'].min()), int(df_main['Year'].max()), key='yr_slider')
    
    st.markdown("<br>", unsafe_allow_html=True)
    st.markdown("**Сортування масиву даних**")
    
    st.checkbox("За зростанням (Ascending)", key='sort_up')
    st.checkbox("За спаданням (Descending)", key='sort_down')
    
    if st.session_state.sort_up and st.session_state.sort_down:
        st.warning("Обрано взаємовиключні параметри. Сортування вимкнено.")
        
    st.markdown("<br>", unsafe_allow_html=True)
    st.button("Відновити за замовчуванням ↺", on_click=hard_reset, use_container_width=True)

with col_display:
    df_focus = df_main[
        (df_main['Region'] == st.session_state.target_reg) &
        (df_main['Year'].between(*st.session_state.yr_slider)) &
        (df_main['Week'].between(*st.session_state.wk_slider))
    ].copy()
    
    if st.session_state.sort_up and not st.session_state.sort_down:
        df_focus.sort_values(by=st.session_state.target_idx, ascending=True, inplace=True)
    elif st.session_state.sort_down and not st.session_state.sort_up:
        df_focus.sort_values(by=st.session_state.target_idx, ascending=False, inplace=True)
        
    if not df_focus.empty:
        mean_val = df_focus[st.session_state.target_idx].mean()
        max_val = df_focus[st.session_state.target_idx].max()
        min_val = df_focus[st.session_state.target_idx].min()
        
        m1, m2, m3 = st.columns(3)
        m1.metric(f"Середній {st.session_state.target_idx}", f"{mean_val:.2f}")
        m2.metric(f"Максимум", f"{max_val:.2f}")
        m3.metric(f"Мінімум", f"{min_val:.2f}")
        st.markdown("<br>", unsafe_allow_html=True)

    tab_data, tab_trend, tab_compare = st.tabs(["Зведена таблиця", "Тренд індексу", "Регіональний рейтинг"])
    
    with tab_data:
        st.dataframe(df_focus[['Year', 'Week', 'Region', 'VCI', 'TCI', 'VHI']], 
                     use_container_width=True, hide_index=True)
                     
    with tab_trend:
        if not df_focus.empty:
            df_trend = df_focus.sort_values(['Year', 'Week']).copy()
            df_trend['Timeline'] = df_trend['Year'].astype(str) + " - W" + df_trend['Week'].astype(str).str.zfill(2)
            
            fig_area = px.area(
                df_trend, x='Timeline', y=st.session_state.target_idx,
                title=f"<b>Динаміка показника {st.session_state.target_idx}</b> | {st.session_state.target_reg}",
                template="plotly_white"
            )
            fig_area.update_traces(
                line_shape='spline', 
                line=dict(color='#636EFA', width=3),
                fillcolor='rgba(99, 110, 250, 0.15)'
            )
            fig_area.update_layout(
                hovermode="x unified",
                xaxis_title="<b>Період (Рік - Тиждень)</b>",
                yaxis_title="<b>Значення індексу</b>",
                plot_bgcolor='rgba(0,0,0,0)'
            )
            st.plotly_chart(fig_area, use_container_width=True)
        else:
            st.info("Бракує даних для візуалізації.")
            
    with tab_compare:
        df_all = df_main[
            (df_main['Year'].between(*st.session_state.yr_slider)) &
            (df_main['Week'].between(*st.session_state.wk_slider))
        ]
        
        if not df_all.empty:
            df_agg = df_all.groupby('Region')[st.session_state.target_idx].mean().reset_index()
            df_agg = df_agg.sort_values(by=st.session_state.target_idx, ascending=True) 
            
            df_agg['Highlight'] = np.where(df_agg['Region'] == st.session_state.target_reg, 'Target', 'Other')
                       
            fig_bar = px.bar(
                df_agg, y='Region', x=st.session_state.target_idx, color='Highlight',
                orientation='h',
                color_discrete_map={'Target': '#10B981', 'Other': '#CBD5E1'},
                title=f"<b>Рейтинг областей: Середній {st.session_state.target_idx}</b>",
                template="plotly_white",
                text_auto='.2f'
            )
            fig_bar.update_traces(textposition='outside')
            fig_bar.update_layout(
                showlegend=False, 
                yaxis_title="", 
                xaxis_title="", 
                height=700,
                plot_bgcolor='rgba(0,0,0,0)'
            )
            st.plotly_chart(fig_bar, use_container_width=True)