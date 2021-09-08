#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int main(int argc, char *argv[]) {
    uint16_t port = 0;
    pcap_t *handle = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct bpf_program fp;
    int res = 0;
    struct pcap_pkthdr *pkt_hdr = NULL;
    uint8_t *pkt = NULL;
    struct ether_header *eth_hdr = NULL;
    struct iphdr *ip_hdr = NULL;
    struct tcphdr *tcp_hdr = NULL;
    char *filter = NULL;

    if(argc != 3) {
        printf("Usage: %s network_interface server_port\n", argv[0]);
        return 1;
    }

    port = atoi(argv[2]);
    if(port < 1 || port > 65535) {
        fprintf(stderr, "%s is not valid port!", argv[2]);
        return 1;
    }
    filter = (char*)calloc(sizeof(int), 30);
    sprintf(filter, "tcp.len>0&&tcp.srcport==%d", port);
    port = htons(port);

    if(pcap_lookupnet(argv[1], &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Failed to get netmask for %s: %s\n", argv[1], errbuf);
        return 1;
    }
    handle = pcap_open_live(argv[1], BUFSIZ, 1, 1000, errbuf);
    if(handle == NULL) {
        fprintf(stderr, "Failed to open pcap handle on %s: %s\n", argv[1], errbuf);
        return 1;
    }
    else if(pcap_compile(handle, &fp, filter, 0, net) == -1) {
        fprintf(stderr, "Failed compile filter %s: %s\n", filter, pcap_geterr(handle));
        return 1;
    }
    else if(pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Failed to set filter on handle: %s\n", pcap_geterr(handle));
        return 1;
    }

    while(1) {
        res = pcap_next_ex(handle, &pkt_hdr, &pkt);
        if(res == 0) {
            continue;
        }
        else if(res == PCAP_ERROR || res == PCAP_ERROR_BREAK) {
            fprintf(stderr, "Failed to get packet from handle.\n");
            return 1;
        }

        if(pkt_hdr->caplen < 1)
            continue;

        eth_hdr = (struct ehter_header*)pkt;
        if(eth_hdr->ether_type != ETHERTYPE_IP) {
            continue;
        }
        pkt += sizeof(*eth_hdr);

        ip_hdr = pkt;
        if(ip_hdr->protocol != IPPROTO_TCP) {
            continue;
        }
        pkt += ip_hdr->ihl * 4;

        tcp_hdr = pkt;
        if(tcp_hdr->source != port) {
            continue;
        }
        else if(tcp_hdr->syn && tcp_hdr->fin && tcp_hdr->rst) {
            continue;
        }
        pkt += tcp_hdr->doff * 4;

    }
    return 0;
}
