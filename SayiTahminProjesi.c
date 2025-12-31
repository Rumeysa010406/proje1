#include <stdio.h>  //printf,scanf,FILE
#include <stdlib.h> //rand,srand, rastgele sayı üretir
#include <time.h>   //time
#include <string.h> //strcpy

int asalMi(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void skorlariListele() {
    FILE *dosya = fopen("kayitlar.txt", "r"); //dosya okuyor,değiştirmiyor,silmiyor.
    char satir[255];

    printf("\n--- GECMIS OYUN KAYITLARI ---\n");
    if (dosya == NULL) {
        printf("Henuz kayitli bir oyun bulunamadi!\n");
        printf("(Ilk oyununuzu oynayarak skor tablosu olusturabilirsiniz)\n");
        printf("-----------------------------\n");
        return;
    }

    int kayitVar = 0;
    while (fgets(satir, sizeof(satir), dosya)) {  //fgets:satır satır okur
        printf("%s", satir);
        kayitVar = 1;
    }

    if (!kayitVar) {
        printf("Henuz kayitli bir oyun bulunamadi!\n");
    }

    fclose(dosya);
    printf("-----------------------------\n");
}

void oyunuBaslat() {
    char isim[50];
    int zorluk, ustSinir, hedefSayi, tahmin;
    int puan = 100, deneme = 0;
    int ipucuSayaci = 0; 
    int gecmisTahminler[1000];
    
    printf("\nOyuncu adinizi girin: ");
    scanf("%s", isim); //isim zaten adres olduğu için & kullanılmaz

    printf("\nZorluk Secin: 1-kolay(1-10), 2-orta(1-100), 3-zor(1-1000): ");
    scanf("%d", &zorluk);
    
    ustSinir = (zorluk == 1) ? 10 : (zorluk == 2 ? 100 : 1000);

    srand(time(NULL)); //her seferinde farklı sayılar üretilmesini sağlar
    hedefSayi = rand() % ustSinir + 1; //1 ile ustSinir arasında rastgele bir sayı üretip hedefSayi değişkenine atar.

    printf("\n%d ile %d arasinda bir sayi tuttum. Baslangic Puani: 100\n", 1, ustSinir);

    while (puan > 0) {
        printf("\nTahmin (Ipucu icin -1 girin, Kalan Ipucu Hakki: %d): ", 1 - ipucuSayaci);
        scanf("%d", &tahmin);

        if (tahmin == -1) {
            if (ipucuSayaci < 1) {
                if (puan >= 10) {
                    puan -= 10;
                    ipucuSayaci++;
                    printf("\n[IPUCU 1/1] Durum:\n", ipucuSayaci);
                    printf("- Sayi %s\n", (asalMi(hedefSayi) ? "ASALDIR" : "ASAL DEGILDIR"));
                    printf("- Sayi 2'nin kati mi? %s\n", (hedefSayi % 2 == 0 ? "EVET (Cift)" : "HAYIR (Tek)"));
                    printf("- Sayi 5'in kati mi? %s\n", (hedefSayi % 5 == 0 ? "EVET" : "HAYIR"));
                    printf("Yeni Puanin: %d\n", puan);
                    continue;
                } else {
                    printf("Ipucu almak icin yeterli puanin yok (En az 10 puan gerekli)!\n");
                    continue;
                }
            } else {
                printf("Uzgunum, tum ipucu haklarini (1/1) kullandin!\n");
                continue;
            }
        }

    
        if (tahmin < 1 || tahmin > ustSinir) {
            printf("SOGUK! Aralik disindasin.\n"); 
            continue;
        }

      
        int tekrar = 0;
        for(int i = 0; i < deneme; i++) {
            if(gecmisTahminler[i] == tahmin) { //Dizideki herhangi bir eski tahmin, yeni girilen tahmine eşit mi diye bakar.
                tekrar = 1; 
                break; //Tekrar bulunduğu anda döngüyü boşuna devam etmemek için durdurur.
            }
        }
        if(tekrar) { 
            printf("Bu sayiyi zaten denedin!\n"); 
            continue; 
        }

      
        gecmisTahminler[deneme] = tahmin;
        deneme++;

        if (tahmin == hedefSayi) {
            char unvan[30];
            if (deneme <= 2) strcpy(unvan, "Keskin Nisanci");
            else if (deneme <= 5) strcpy(unvan, "Usta Tahminci");
            else strcpy(unvan, "Acemi"); //kaynak(acemi) string → hedef(unvan) stringe kopyalanır

            printf("\n=== TEBRIKLER! ===\n");
            printf("Dogru Sayi: %d\n", hedefSayi);
            printf("Puan: %d\n", puan);
            printf("Unvan: %s\n", unvan);
            printf("Deneme Sayisi: %d\n", deneme);
            printf("Kullanilan Ipucu Sayisi: 1\n", ipucuSayaci); 
            printf("==================\n");

            FILE *dosya = fopen("kayitlar.txt", "a");
            if (dosya != NULL) {
                time_t simdi = time(NULL);  //Şu anki zamanı alır.
                struct tm *zaman = localtime(&simdi);  //Alınan zamanı yerel saat dilimine göre parçalar
                char tarihSaat[50];  //Tarih–saat bilgisini tutacak karakter dizisi (string).
                strftime(tarihSaat, sizeof(tarihSaat), "%Y-%m-%d %H:%M:%S", zaman); //Tarih ve saati formatlı bir stringe çevirir.
                                    //Yıl,Ay,Gün - Saat,Dakika,Saniye
                fprintf(dosya, "Oyuncu: %-10s | Puan: %-3d | Deneme: %-2d | Unvan: %-15s | Ipucu: %-2d | Tarih: %s\n", 
                        isim, puan, deneme, unvan, ipucuSayaci, tarihSaat);
                fflush(dosya); //Dosyaya yazılmış ama henüz gerçekten diske gitmemiş verileri hemen yazdırır.
                fclose(dosya); //Açılmış olan dosyayı kapatır ve dosya ile yapılan işlemi güvenli şekilde bitirir.
                printf("Skorun basariyla kaydedildi!\n");
            } else {
                printf("UYARI: Skor kaydedilemedi!\n");
                printf("Sebep: Dosya olusturulamadi veya yazma izni yok.\n");
                printf("Cozum: Programi yonetici olarak calistirmayi deneyin.\n");
            }
            return;
        } else {
            puan -= 10;
            printf("%s! Kalan Puan: %d | Deneme: %d\n", 
                   (tahmin < hedefSayi ? "YUKARI" : "ASAGI"), puan, deneme);
        }
    }
    
    printf("\n=== OYUN BITTI ===\n");
    printf("Puanin bitti! Kaybettin.\n");
    printf("Tutulan sayi: %d\n", hedefSayi);
    printf("Toplam Deneme: %d\n", deneme);
    printf("Kullanilan Ipucu Sayisi: %d\n", ipucuSayaci); 
    printf("==================\n");
}

int main() {
    int secim;
    do {
        printf("\n=== GELISTIRILMIS SAYI TAHMIN OYUNU ===\n");
        printf("1. Oyunu Baslat\n");
        printf("2. Skor Tablosunu Gor\n");
        printf("3. Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);

        switch(secim) {
            case 1: oyunuBaslat(); break;
            case 2: skorlariListele(); break;
            case 3: printf("Hoscakal!\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while(secim != 3);
                                                                                                
    return 0;
}
